#!/usr/bin/env bash
# Build Chordy for Android (arm64-v8a) usando Qt for Android.
#
# Requisitos previos:
#   - Qt for Android instalado en ~/Qt/<version>/android_arm64_v8a
#       Instalación recomendada (1 vez):
#         HOME="$(pwd)/.aqt-home" ./.venv-aqt/bin/aqt install-qt \
#             mac android 6.9.3 android_arm64_v8a -O "$HOME/Qt"
#   - Android SDK en ~/Library/Android/sdk (ya lo dejó Android Studio)
#   - NDK dentro del SDK (~/Library/Android/sdk/ndk/<version>)
#   - JDK: usa el JBR embebido en Android Studio si no hay otro
set -euo pipefail
cd "$(dirname "$0")"

QT_BASE="${QT_BASE:-$HOME/Qt}"
if [ -z "${QT_ANDROID_DIR:-}" ]; then
  QT_ANDROID_DIR=$(ls -d "$QT_BASE"/*/android_arm64_v8a 2>/dev/null | sort -V | tail -1 || true)
fi
if [ -z "${QT_ANDROID_DIR:-}" ] || [ ! -d "$QT_ANDROID_DIR" ]; then
  echo "ERROR: no encuentro Qt for Android arm64_v8a en $QT_BASE/*/android_arm64_v8a"
  echo
  echo "  Para instalarlo (1 vez, ~1.5 GB):"
  echo "    cd $(pwd)"
  echo "    HOME=\"\$(pwd)/.aqt-home\" ./.venv-aqt/bin/aqt install-qt \\"
  echo "        mac android 6.9.3 android_arm64_v8a -O \"\$HOME/Qt\""
  echo
  echo "  (si 6.9.3 no estuviera disponible en el mirror, probar 6.9.2 o 6.9.0)"
  exit 1
fi

QT_CMAKE="$QT_ANDROID_DIR/bin/qt-cmake"
if [ ! -x "$QT_CMAKE" ]; then
  echo "ERROR: $QT_CMAKE no es ejecutable"
  exit 1
fi
echo "[abuild] Qt for Android: $QT_ANDROID_DIR"

# Qt escritorio (host): misma línea 6.9.x que el kit Android (moc, rcc, uic desde macOS).
QT_VER_ANDROID="$(basename "$(dirname "$QT_ANDROID_DIR")")"

_is_qt6_host_ok() {
  [[ -n "${1:-}" && -f "$1/lib/cmake/Qt6/Qt6Config.cmake" ]]
}

_abs_prefix_from_qt6config() {
  # .../PREFIX/lib/cmake/Qt6/Qt6Config.cmake → PREFIX
  dirname "$(dirname "$(dirname "$(dirname "$1")")")"
}

# Encuentra un Qt6 escritorio dentro de ROOT (excluye android/ios).
_pick_mac_qt_under() {
  local root="$1"
  [[ -z "$root" ]] || [[ ! -d "$root" ]] && return 1
  local cfg
  if _is_qt6_host_ok "$root"; then
    printf '%s\n' "$root"
    return 0
  fi
  while IFS= read -r cfg; do
    case "$cfg" in
      */android_arm*|*/android_*|*/ios/*|*Ios*|*Simulator*) continue ;;
    esac
    printf '%s\n' "$(_abs_prefix_from_qt6config "$cfg")"
    return 0
  done < <(find "$root" -type f -path "*/lib/cmake/Qt6/Qt6Config.cmake" \
    ! -path "*/android_arm*" ! -path "*/android_x86*" 2>/dev/null)
  return 1
}

# 1) Respetar QT_HOST_PATH; si incompleto, explorar ese árbol.
if [[ -n "${QT_HOST_PATH:-}" ]] && ! _is_qt6_host_ok "${QT_HOST_PATH}"; then
  RESOLVED="$(_pick_mac_qt_under "${QT_HOST_PATH}")"
  QT_HOST_PATH="${RESOLVED:-}"
fi

# 2) Vacío → Homebrew (suele ser /opt/homebrew/opt/qt junto al tu Android ~/Qt).
if [[ -z "${QT_HOST_PATH:-}" ]]; then
  for cand in "/opt/homebrew/opt/qt" "/usr/local/opt/qt"; do
    if _is_qt6_host_ok "$cand"; then
      QT_HOST_PATH="$cand"
      break
    fi
  done
fi

# 3) Vacío → instalador oficial: misma versión en ~/Qt o /Applications/Qt
if [[ -z "${QT_HOST_PATH:-}" && -n "$QT_VER_ANDROID" ]]; then
  for root in "$HOME/Qt/$QT_VER_ANDROID" "/Applications/Qt/$QT_VER_ANDROID"; do
    QT_HOST_PATH="$(_pick_mac_qt_under "$root")" || QT_HOST_PATH=""
    [[ -n "${QT_HOST_PATH:-}" ]] && break
  done
fi

if ! _is_qt6_host_ok "${QT_HOST_PATH:-}"; then
  echo "ERROR: Qt 6 host (Desktop macOS) no encontrado para versión $QT_VER_ANDROID."
  echo "  Necesitás el mismo Qt en macOS para compilar contra el kit Android:"
  echo "    brew install qt"
  echo "  o instalá desde Qt Maintenance Tool los componentes *Desktop / macOS* de $QT_VER_ANDROID."
  echo "  Solo tener Android ARM en ~/Qt no alcanza: hace falta el kit compilado para Mac."
  [[ -n "${QT_HOST_PATH:-}" ]] && echo "(último intento: QT_HOST_PATH=$QT_HOST_PATH)"
  exit 1
fi
echo "[abuild] QT_HOST_PATH       = $QT_HOST_PATH"

# Android SDK
export ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-$HOME/Library/Android/sdk}"
export ANDROID_HOME="$ANDROID_SDK_ROOT"
if [ ! -d "$ANDROID_SDK_ROOT" ]; then
  echo "ERROR: ANDROID_SDK_ROOT=$ANDROID_SDK_ROOT no existe"
  exit 1
fi

# NDK más reciente disponible en el SDK
if [ -z "${ANDROID_NDK_ROOT:-}" ]; then
  ANDROID_NDK_ROOT=$(ls -d "$ANDROID_SDK_ROOT/ndk"/*/ 2>/dev/null | sort -V | tail -1 | sed 's:/*$::' || true)
fi
if [ -z "${ANDROID_NDK_ROOT:-}" ] || [ ! -d "$ANDROID_NDK_ROOT" ]; then
  echo "ERROR: no encuentro NDK en $ANDROID_SDK_ROOT/ndk/"
  echo "  Instalalo desde Android Studio: SDK Manager > SDK Tools > NDK (Side by side)"
  exit 1
fi
export ANDROID_NDK_ROOT
export ANDROID_NDK_HOME="$ANDROID_NDK_ROOT"

# JDK: preferir JAVA_HOME si está, si no usar el JBR de Android Studio
if [ -z "${JAVA_HOME:-}" ]; then
  JBR="/Applications/Android Studio.app/Contents/jbr/Contents/Home"
  if [ -d "$JBR" ]; then
    export JAVA_HOME="$JBR"
  else
    echo "ERROR: no hay JAVA_HOME ni JBR de Android Studio"
    echo "  Instalar JDK 17: brew install --cask temurin@17"
    exit 1
  fi
fi
export PATH="$JAVA_HOME/bin:$PATH"

echo "[abuild] ANDROID_SDK_ROOT = $ANDROID_SDK_ROOT"
echo "[abuild] ANDROID_NDK_ROOT = $ANDROID_NDK_ROOT"
echo "[abuild] JAVA_HOME        = $JAVA_HOME"
echo

BUILD_DIR="${BUILD_DIR:-build-android-arm64}"
mkdir -p "$BUILD_DIR"

"$QT_CMAKE" -S . -B "$BUILD_DIR" \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DANDROID_SDK_ROOT="$ANDROID_SDK_ROOT" \
  -DANDROID_NDK_ROOT="$ANDROID_NDK_ROOT" \
  -DQT_HOST_PATH="$QT_HOST_PATH" \
  -DQT_ANDROID_BUILD_ALL_ABIS=OFF \
  -DQT_ANDROID_ABIS=arm64-v8a

cmake --build "$BUILD_DIR" --parallel

# Empaqueta APK debug (Qt corre Gradle para nosotros).
cmake --build "$BUILD_DIR" --target apk

echo
echo "OK. APK debug:"
find "$BUILD_DIR/android-build" -name "*.apk" -path "*/debug/*" 2>/dev/null || true
echo
echo "Para instalar y lanzar en device/emulador conectado:  ./arun.sh"
echo "Para abrir el proyecto Gradle en Android Studio:       ./aopen.sh"
