#include <iostream>
#include <set>
#include "chordy.h"



Chordy::Chordy(QWidget *parent)
    : QMainWindow(parent)
{
}

bool Chordy::create(const std::string &title)
{
    setWindowTitle(title.c_str());
    resize(320, 580);

    setStyleSheet("QMainWindow { background-color: rgb(230, 230, 230); }");

    try
    {
        pGrid = new ChordGrid(this, 5);
    }
    catch(const std::bad_alloc& e)
    {
        std::cout << e.what() << '\n';
        return false;
    }

	try
	{
		pDoFret = new QPushButton(this);
	}
	catch(const std::bad_alloc& e)
	{
		std::cout << e.what() << '\n';
		return false;
	}
    
    if(pGrid->create())
    {
        setCentralWidget(pGrid);

        connect(pGrid, &ChordGrid::gridChanged,
                this,  &Chordy::onGridChanged);

		connect(pDoFret, &QPushButton::clicked,
				pGrid, &ChordGrid::doFret);

		pDoFret->setText("Do a fret");

		pDoFret->setFixedSize(100, 30);
        pDoFret->move(ChordGrid::left -2, (ChordGrid::top - pDoFret->height()) - 5);
		pDoFret->setStyleSheet(
			"QPushButton {"
			"  background-color: rgb(200, 200, 200);"
			"  color: rgb(0, 0, 0);"
			"  border: 1px solid rgb(200, 200, 200);"
			"  border-radius: 15px;"  // ~mitad de la altura (30) para forma de pastilla
			"  padding: 5px;"
			"  font-size: 12px;"
			"  font-weight: bold;"
			"  font-family: Arial, sans-serif;"
			"}");
        
		// Ejemplo: Em9/11 desde traste VI
        // cuerdas: 0=Mi grave ... 5=Mi agudo
        // O=open, X=muted
        /*
        std::array<ChordGrid::TopMarker, 6> markers = {
            ChordGrid::Open,  // Mi grave (cuerda 6)
            ChordGrid::None,
            ChordGrid::None,
            ChordGrid::None,
            ChordGrid::None,
            ChordGrid::Open   // Mi agudo (cuerda 1)
        };

        std::vector<ChordGrid::Dot> dots = {
            {1, 2},  // cuerda 5, traste 2 del diagrama
            {3, 3},  // cuerda 3, traste 3
            {2, 4},  // cuerda 4, traste 4 — ajustá según el acorde real
            {4, 4},  // cuerda 2, traste 4
        };

        connect(pGrid, &ChordGrid::gridChanged,
                this,  &Chordy::onGridChanged);

        // barre parcial en fila 1, cuerdas 1 a 5
        pGrid->setChord("Em9/11", 6, markers, dots, 1, 5, 0);
        */
    }

    return true;
}

void Chordy::onGridChanged(int startFret,
                            std::array<ChordGrid::TopMarker, ChordGrid::totalStrings> markers,
                            std::vector<ChordGrid::Dot> dots)
{
    pGrid->setName(detectChord(startFret, markers, dots));
}

QString Chordy::detectChord(int startFret,
                              const std::array<ChordGrid::TopMarker, ChordGrid::totalStrings> &markers,
                              const std::vector<ChordGrid::Dot> &dots) const
{
    // Afinación estándar: E A D G B E (clases de tono, 0=C)
    static const int openNote[6] = { 4, 9, 2, 7, 11, 4 };

    static const char *noteName[] = {
        "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"
    };

    struct Template {
        const char   *suffix;
        std::vector<int> iv; // intervalos desde la raíz
    };
    static const Template templates[] = {
        { "",      { 0, 4, 7 }           },  // Mayor
        { "m",     { 0, 3, 7 }           },  // Menor
        { "7",     { 0, 4, 7, 10 }       },  // Dom 7
        { "maj7",  { 0, 4, 7, 11 }       },  // Maj 7
        { "m7",    { 0, 3, 7, 10 }       },  // m7
        { "m7b5",  { 0, 3, 6, 10 }       },  // semidisminuido
        { "dim",   { 0, 3, 6 }           },  // Disminuido
        { "dim7",  { 0, 3, 6, 9 }        },  // Dim 7
        { "aug",   { 0, 4, 8 }           },  // Aumentado
        { "sus2",  { 0, 2, 7 }           },
        { "sus4",  { 0, 5, 7 }           },
        { "6",     { 0, 4, 7, 9 }        },
        { "m6",    { 0, 3, 7, 9 }        },
        { "add9",  { 0, 2, 4, 7 }        },
        { "9",     { 0, 2, 4, 7, 10 }    },
        { "m9",    { 0, 2, 3, 7, 10 }    },
    };

    // Recolectar clases de tono activas
    std::set<int> pcs;
    for (int s = 0; s < ChordGrid::totalStrings; ++s)
        if (markers[s] == ChordGrid::Open)
            pcs.insert(openNote[s]);

    for (const auto &dot : dots)
        pcs.insert((openNote[dot.string] + (startFret - 1) + dot.fret) % 12);

    if (pcs.empty())
        return {};

    // Probar cada clase de tono como raíz y buscar la plantilla más específica
    QString best;
    int bestScore = 0;

    for (int root : pcs)
    {
        std::set<int> intervals;
        for (int pc : pcs)
            intervals.insert((pc - root + 12) % 12);

        for (const auto &t : templates)
        {
            bool match = true;
            for (int i : t.iv)
                if (!intervals.count(i)) { match = false; break; }

            if (match && static_cast<int>(t.iv.size()) > bestScore)
            {
                bestScore = static_cast<int>(t.iv.size());
                best = QString("%1%2").arg(noteName[root]).arg(t.suffix);
            }
        }
    }

    return best.isEmpty() ? "?" : best;
}
