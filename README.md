# Chordy

Aplicación para crear diagramas de acordes de guitarra de forma interactiva. Identifica el nombre del acorde en tiempo real a medida que se colocan o retiran dedos sobre la cuadrícula del mástil.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Qt](https://img.shields.io/badge/Qt-5%20%7C%206-green)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux%20%7C%20Windows%20%7C%20Android-lightgrey)

## Características

- **Diagrama interactivo** — Cuadrícula de 6 cuerdas × 5 trastes donde se añaden o quitan puntos (dedos) con un clic.
- **Detección de acordes en tiempo real** — Cada cambio en la cuadrícula recalcula las notas activas y muestra el nombre del acorde detectado, incluyendo inversiones como `C/E`.
- **Soporte de afinación estándar** — E A D G B E (Mi grave a Mi agudo).
- **Marcadores de cuerda** — Indicadores de cuerda al aire (O) y cuerda muteada (X) sobre el diagrama.
- **Cejilla y posición** — Soporte para cejilla (barre), desplazamiento del traste inicial y visualización del número romano correspondiente.
- **Copiar al portapapeles** — Exporta el acorde armado como texto ASCII y como imagen PNG compacta en el mismo portapapeles; la aplicación destino elige el formato al pegar.

## Acordes reconocidos

El motor de detección trabaja con las 12 tonalidades, evalúa la nota más grave como bajo y puede devolver inversiones en formato slash chord (`C/E`, `Am/G`, etc.). Actualmente reconoce estas familias:

| Tipo | Ejemplo |
|------|---------|
| Mayor | C |
| Menor | Am |
| Séptima dominante | G7 |
| Séptima mayor | Cmaj7 |
| Séptima menor | Dm7 |
| Menor séptima bemol quinta | Bm7b5 |
| Disminuido | Bdim |
| Disminuido séptima | Bdim7 |
| Aumentado | Caug |
| Suspendido 2ª | Dsus2 |
| Suspendido 4ª | Asus4 |
| Sexta | C6 |
| Sexta menor | Am6 |
| Add9 | Cadd9 |
| Novena | G9 |
| Novena menor | Am9 |
| Novena mayor | Cmaj9 |

La detección no depende de una lista fija de posiciones de guitarra: primero calcula las notas que suenan en la grilla y luego compara ese conjunto contra plantillas de intervalos. Esto permite reconocer el mismo acorde en diferentes posiciones del mástil.

## Requisitos

- **CMake** ≥ 3.16
- **Qt 5** o **Qt 6** (módulos: Core, Widgets)
- **Compilador C++17** (GCC, Clang o Apple Clang)
- **macOS** (Homebrew) o **Linux**

## Compilar y ejecutar

```bash
# Clonar el repositorio
git clone git@github.com:lsayanes/Chordy.git
cd Chordy

# Compilar (detecta Qt6 vía Homebrew, fallback a Qt5)
./build.sh

# Ejecutar
./run.sh

# O compilar y ejecutar en un solo paso
./brun.sh
```

Para limpiar los artefactos de compilación:

```bash
./clean.sh
```

## Compilar para Android (Qt for Android)

El mismo código C++ se empaqueta como APK arm64-v8a. La build se hace con `qt-cmake` apuntando al kit Android de Qt 6, y el bundle Gradle resultante se puede abrir en Android Studio para hacer debug, perfilar e instalar en device.

### Requisitos

- macOS Apple Silicon o Intel (también Linux con ajustes en los scripts).
- **Android Studio** (provee el SDK + NDK + JDK 21 embebido).
  - SDK en `~/Library/Android/sdk` (default de Android Studio).
  - NDK 25 / 26 / 27 / 29 cualquiera dentro de `~/Library/Android/sdk/ndk/`.
- **Qt for Android 6.9.x** instalado en `~/Qt/6.9.x/android_arm64_v8a`.
- **Python 3.10+** (para `aqtinstall`, el descargador de Qt).

### Instalar Qt for Android (1 sola vez)

Se baja por CLI vía `aqtinstall`, en un venv local del repo (no toca Python del sistema). Pesa ~1.5 GB.

```bash
cd /ruta/a/Chordy
python3 -m venv .venv-aqt
.venv-aqt/bin/pip install --upgrade pip aqtinstall

# El HOME=... redirige el cache de aqt a un dir local del repo (no escribe en
# ~/Library/Application Support, útil en sandboxes).
mkdir -p .aqt-home
HOME="$(pwd)/.aqt-home" ./.venv-aqt/bin/aqt install-qt \
    mac android 6.9.3 android_arm64_v8a -O "$HOME/Qt"
```

> Si el mirror no tiene exactamente `6.9.3`, probar `6.9.2` o `6.9.0`. La versión Qt-Android **debe ser igual a la versión Qt host** que tengas en macOS (verificable con `qmake6 -query QT_VERSION`).

### Build APK

```bash
./abuild.sh
```

Lo que hace:
- Detecta el Qt for Android instalado en `~/Qt/*/android_arm64_v8a` (el más nuevo).
- **Configura `QT_HOST_PATH`** buscando: (1) variable de entorno, (2) Homebrew `/opt/homebrew/opt/qt`, (3) `~/Qt/<versión>/` y `/Applications/Qt/<versión>/` (nombre del kit distinto de `macos`). Qt 6 exige Qt *host*: el kit Android es sólo cross-compile; moc/rcc/uic deben ejecutarse desde un Qt compilado para macOS.
- Exporta `ANDROID_SDK_ROOT`, `ANDROID_NDK_ROOT` (NDK más nuevo del SDK) y `JAVA_HOME` (JBR de Android Studio si no hay otro).
- Configura con `qt-cmake -DQT_ANDROID_ABIS=arm64-v8a` en `build-android-arm64/`.
- Corre `cmake --build` y luego `--target apk` (Qt invoca Gradle internamente).

El APK debug queda en:
```
build-android-arm64/android-build/Chordy/build/outputs/apk/debug/Chordy-debug.apk
```

### Fallos típicos (CMake / Qt host)

- **`QT_HOST_PATH` y `/Applications/Qt/.../macos`:** esa carpeta a veces no existe; instalá también el kit **Desktop (macOS)** en el Maintenance Tool para la misma 6.9.x, o usá **`brew install qt`** y no exportes `QT_HOST_PATH`.
- **`target_link_libraries` plain vs keyword:** el proyecto usa `PRIVATE`; si mezclas firma plain con la que aplica Qt al finalizar el target Android, CMake falla (ya cubierto en `CMakeLists.txt`).

### Instalar / lanzar en device o emulador

Conectá el teléfono por USB con depuración activada (o arrancá un AVD desde Android Studio) y:

```bash
./arun.sh
```

Hace `adb install -r` del APK y lanza la `QtActivity`.

### Abrir en Android Studio

```bash
./aopen.sh
```

Abre Android Studio sobre `build-android-arm64/android-build/`, que **es un proyecto Gradle válido**. Desde ahí podés:

- Build / Run / Profile como cualquier app Android.
- Generar AAB de release (Build → Generate Signed Bundle/APK).
- Debug nativo (LLDB sobre el `.so` del binario Qt).
- Instalar en device / emulador con un click.

### Cosas a tener en cuenta en mobile

- La UI se adapta vía `#ifdef Q_OS_ANDROID`: la franja superior de botones (Barre / Copy) y el carril de marcadores X / O son más altos para tener un target táctil cómodo.
- En Android la app llena la pantalla (`showMaximized()`); el grid escala con `resizeEvent` igual que en desktop.
- El portapapeles funciona para texto en cualquier app. Para imágenes PNG, no todas las apps Android consumen `image/png` desde clipboard (es una limitación del OS, no del código). El texto siempre se pega bien.
- `arm64-v8a` cubre 100 % de teléfonos modernos; si necesitás emulador x86_64 en Mac Intel o devices viejos armv7, instalá esos targets adicionales con `aqt install-qt ... android_x86_64` / `android_armv7` y agregalos a `QT_ANDROID_ABIS` en `abuild.sh`.

## Estructura del proyecto

```
Chordy/
├── main.cpp                 # Punto de entrada, configura QApplication (Fusion)
├── chordy.h/.cpp            # Ventana principal (QMainWindow), conexión de señales,
│                            #   detección de acordes por plantillas de intervalos
├── chordGrid.h/.cpp         # Widget del diagrama: dibujado, interacción con ratón,
│                            #   gestión de puntos, marcadores y cejilla
├── chordy.qrc               # Recursos empaquetados en el binario (ícono)
├── CMakeLists.txt           # Build dual desktop (Qt5/Qt6) + Android (Qt6)
├── build.sh / brun.sh / run.sh / clean.sh   # Scripts desktop
├── abuild.sh / arun.sh / aopen.sh            # Scripts Android (Qt for Android)
├── android/                 # Package source para Qt + AndroidManifest.xml + iconos
│   ├── AndroidManifest.xml
│   └── res/mipmap-xxxhdpi/icon.png
├── resources/               # Recursos en disco (también disponibles vía qrc)
└── README.md
```

## Cómo funciona

1. **Interacción:** Al hacer clic en una celda de la cuadrícula se alterna un punto (dedo) en esa posición cuerda/traste.
2. **Cejilla:** El botón **Do a fret** alterna una cejilla usando los dos últimos puntos colocados en el mismo traste como extremos; al activarla, esos puntos se reemplazan visualmente por una barra.
3. **Cálculo de notas:** Se determina qué nota suena en cada cuerda: cuerda muteada no aporta nota, un punto o cejilla pisa la cuerda, y una cuerda sin pisar suena al aire. La nota se calcula como pitch class módulo 12.
4. **Detección:** Se prueban las notas activas como posibles tónicas y se comparan contra plantillas de intervalos. El algoritmo puntúa coincidencias, notas faltantes y notas extra, con un pequeño bonus cuando la tónica coincide con el bajo.
5. **Resultado:** El nombre del acorde se muestra debajo del diagrama. Si la nota más grave no es la tónica detectada, se muestra como inversión (`Raíz/Bajo`).
6. **Copia:** El botón **Copiar** coloca en el portapapeles una versión ASCII del diagrama y una imagen PNG renderizada desde el mismo estado de la grilla.


<video src="https://github.com/user-attachments/assets/30596252-275d-4024-b447-1a851e0e57a3" controls="controls" width="100%" height="auto"></video>



