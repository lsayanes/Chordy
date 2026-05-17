#!/usr/bin/env bash
# Instala el APK debug en device/emulador conectado y lanza Chordy.
set -euo pipefail
cd "$(dirname "$0")"

ADB="${ADB:-$HOME/Library/Android/sdk/platform-tools/adb}"
if [ ! -x "$ADB" ]; then
  echo "ERROR: adb no encontrado en $ADB"
  exit 1
fi

PKG="com.idev.chordy"
ACT="org.qtproject.qt.android.bindings.QtActivity"
BUILD_DIR="${BUILD_DIR:-build-android-arm64}"

# Buscar el APK debug que dejó Qt/Gradle
APK_PATH=$(find "$BUILD_DIR/android-build" -path "*/debug/*.apk" 2>/dev/null | head -1)
if [ -z "$APK_PATH" ] || [ ! -f "$APK_PATH" ]; then
  echo "ERROR: APK no encontrado en $BUILD_DIR/android-build/.../debug/"
  echo "  Corré ./abuild.sh primero."
  exit 1
fi

echo "[arun] APK: $APK_PATH"
echo "[arun] Devices conectados:"
"$ADB" devices

"$ADB" install -r "$APK_PATH"
"$ADB" shell am start -n "$PKG/$ACT"
echo
echo "Para seguir logs:"
echo "  $ADB logcat | grep -iE 'chordy|qt'"
