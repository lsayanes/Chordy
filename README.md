# Chordy

Aplicación de escritorio para crear diagramas de acordes de guitarra de forma interactiva. Identifica el nombre del acorde en tiempo real a medida que se colocan o retiran dedos sobre la cuadrícula del mástil.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Qt](https://img.shields.io/badge/Qt-5%20%7C%206-green)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey)
![License](https://img.shields.io/badge/license-MIT-orange)

## Características

- **Diagrama interactivo** — Cuadrícula de 6 cuerdas × 5 trastes donde se añaden o quitan puntos (dedos) con un clic.
- **Detección de acordes en tiempo real** — Cada cambio en la cuadrícula recalcula las notas activas y muestra el nombre del acorde detectado.
- **Soporte de afinación estándar** — E A D G B E (Mi grave a Mi agudo).
- **Marcadores de cuerda** — Indicadores de cuerda al aire (O) y cuerda muteada (X) sobre el diagrama.
- **Cejilla y posición** — Soporte para cejilla (barre) y desplazamiento del traste inicial con número romano.
- **Copiar imagen al portapapeles** — *(en desarrollo)* Permite copiar el diagrama del acorde como imagen para pegarlo en editores de texto u otros programas.

## Acordes reconocidos

El motor de detección identifica los siguientes tipos de acorde en las 12 tonalidades:

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
2. **Cálculo de notas:** Se determina la clase de altura (pitch class) de cada cuerda activa, sumando la nota al aire + posición del traste, módulo 12.
3. **Detección:** Se prueban todas las notas activas como posible tónica y se compara el conjunto de intervalos resultante contra una tabla de plantillas de acordes conocidos. Gana la plantilla con mayor número de intervalos coincidentes.
4. **Resultado:** El nombre del acorde se muestra debajo del diagrama, o `?` si no se encuentra coincidencia.

## Roadmap

- [ ] Copiar diagrama como imagen al portapapeles
- [ ] Interfaz para alternar marcadores O/X por cuerda desde la UI
- [ ] Exportar diagrama como PNG/SVG
- [ ] Más tipos de acordes (slash chords, inversiones, alterados)
- [ ] Soporte para afinaciones alternativas
- [ ] Biblioteca de acordes predefinidos

## Licencia

Este proyecto está bajo la licencia MIT. Consulta el archivo [LICENSE](LICENSE) para más detalles.
