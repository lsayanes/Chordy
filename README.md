# Chordy

Aplicación de escritorio para crear diagramas de acordes de guitarra de forma interactiva. Identifica el nombre del acorde en tiempo real a medida que se colocan o retiran dedos sobre la cuadrícula del mástil.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Qt](https://img.shields.io/badge/Qt-5%20%7C%206-green)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey)

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

## Estructura del proyecto

```
Chordy/
├── main.cpp          # Punto de entrada, configura QApplication (estilo Fusion)
├── chordy.h/.cpp     # Ventana principal (QMainWindow), conexión de señales,
│                     #   detección de acordes por plantillas de intervalos
├── chordGrid.h/.cpp  # Widget del diagrama: dibujado, interacción con ratón,
│                     #   gestión de puntos, marcadores y cejilla
├── CMakeLists.txt    # Sistema de build (Qt5/Qt6, C++17, info de Git)
├── build.sh          # Script de compilación
├── brun.sh           # Compilar + ejecutar
├── run.sh            # Ejecutar binario compilado
├── clean.sh          # Limpiar artefactos
├── resources/        # Recursos de la aplicación
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



