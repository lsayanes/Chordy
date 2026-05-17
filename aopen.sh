#!/usr/bin/env bash
# Abre el proyecto Gradle generado por Qt for Android en Android Studio.
# Lo que abrís es el módulo Gradle que Qt creó dentro de build-android-arm64/android-build/.
# Desde ahí Android Studio te deja: build APK/AAB, instalar en device, debug nativo,
# perfilador y firma de release igual que cualquier app Android.
set -euo pipefail
cd "$(dirname "$0")"

BUILD_DIR="${BUILD_DIR:-build-android-arm64}"
PROJ="$BUILD_DIR/android-build"
if [ ! -d "$PROJ" ]; then
  echo "ERROR: $PROJ no existe. Corré ./abuild.sh primero."
  exit 1
fi

open -a "Android Studio" "$PROJ"
