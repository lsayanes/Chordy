#include <iostream>
#include <set>
#include <limits>
#include <algorithm>
#include "chordy.h"



Chordy::Chordy(QWidget *parent)
    : QMainWindow(parent)
{
}

bool Chordy::create(const std::string &title)
{
    setWindowTitle(title.c_str());
    resize(320, 580);

    setStyleSheet("QMainWindow { background-color: rgb(255, 255, 255); }");

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
		pCopy   = new QPushButton(this);
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

		connect(pCopy, &QPushButton::clicked,
				pGrid, &ChordGrid::copyToClipboard);

		static const char *kPillStyle =
			"QPushButton {"
			"  background-color: rgb(200, 200, 200);"
			"  color: rgb(0, 0, 0);"
			"  border: 1px solid rgb(200, 200, 200);"
			"  border-radius: 15px;"
			"  padding: 5px;"
			"  font-size: 12px;"
			"  font-weight: bold;"
			"  font-family: Arial, sans-serif;"
			"}";

		pDoFret->setText("Do a fret");
		pDoFret->setFixedSize(100, 30);
		pDoFret->move(ChordGrid::left - 2, (ChordGrid::top - pDoFret->height()) - 7);
		pDoFret->setStyleSheet(kPillStyle);

		pCopy->setText("Copiar");
		pCopy->setToolTip("Copiar el diagrama del acorde como texto");
		pCopy->setFixedSize(80, 30);
		pCopy->move(ChordGrid::left - 2 + pDoFret->width() + 6,
					(ChordGrid::top - pCopy->height()) - 7);
		pCopy->setStyleSheet(kPillStyle);
        
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

void Chordy::onGridChanged(int8_t startFret,
                            std::array<ChordGrid::TopMarker, ChordGrid::totalStrings> markers,
                            std::vector<ChordGrid::Dot> dots,
                            int8_t barreFret,
                            int8_t barreFrom,
                            int8_t barreTo)
{
    pGrid->setName(detectChord(startFret, markers, dots, barreFret, barreFrom, barreTo));
}

QString Chordy::detectChord(int startFret,
                              const std::array<ChordGrid::TopMarker, ChordGrid::totalStrings> &markers,
                              const std::vector<ChordGrid::Dot> &dots,
                              int barreFret,
                              int barreFrom,
                              int barreTo) const
{
    // Afinación estándar (cuerda 0=Mi grave ... 5=Mi agudo). 0=C, 1=C#, ... 11=B.
    static const int openNote[ChordGrid::totalStrings] = { 4, 9, 2, 7, 11, 4 };

    static const char *noteName[] = {
        "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"
    };

    struct Template {
        const char       *suffix;
        std::vector<int>  iv;       // intervalos desde la raíz (en semitonos mod 12)
    };
    // Ordenadas de más específicas a menos. Ante igualdad de score gana la más específica.
    static const Template templates[] = {
        { "maj9",  { 0, 2, 4, 7, 11 }    },
        { "9",     { 0, 2, 4, 7, 10 }    },
        { "m9",    { 0, 2, 3, 7, 10 }    },
        { "maj7",  { 0, 4, 7, 11 }       },
        { "m7b5",  { 0, 3, 6, 10 }       },
        { "dim7",  { 0, 3, 6, 9 }        },
        { "m7",    { 0, 3, 7, 10 }       },
        { "7",     { 0, 4, 7, 10 }       },
        { "6",     { 0, 4, 7, 9 }        },
        { "m6",    { 0, 3, 7, 9 }        },
        { "add9",  { 0, 2, 4, 7 }        },
        { "sus2",  { 0, 2, 7 }           },
        { "sus4",  { 0, 5, 7 }           },
        { "dim",   { 0, 3, 6 }           },
        { "aug",   { 0, 4, 8 }           },
        { "m",     { 0, 3, 7 }           },
        { "",      { 0, 4, 7 }           },
    };

    // Paso 1: determinar qué nota suena (si suena) en cada cuerda.
    //  - Muted          -> no suena.
    //  - dot/cejilla    -> mayor de los frets pisados; se aplica startFret-1 como offset.
    //  - resto          -> al aire (cualquier cuerda no muteada y sin nada encima).
    int playedNote[ChordGrid::totalStrings];
    for (int s = 0; s < ChordGrid::totalStrings; ++s)
        playedNote[s] = -1;

    const int barreLo = std::min<int>(barreFrom, barreTo);
    const int barreHi = std::max<int>(barreFrom, barreTo);

    for (int s = 0; s < ChordGrid::totalStrings; ++s)
    {
        if (markers[s] == ChordGrid::Muted)
            continue;

        int highestFret = -1;
        for (const auto &d : dots)
        {
            if (d.string == s && d.fret > highestFret)
                highestFret = d.fret;
        }

        if (barreFret > 0 && s >= barreLo && s <= barreHi && barreFret > highestFret)
            highestFret = barreFret;

        if (highestFret > 0)
            playedNote[s] = (openNote[s] + (startFret - 1) + highestFret) % 12;
        else
            playedNote[s] = openNote[s]; // al aire
    }

    // Pitch classes presentes y bajo (cuerda más grave que suena).
    std::set<int> pcs;
    int bassPc = -1;
    for (int s = 0; s < ChordGrid::totalStrings; ++s)
    {
        if (playedNote[s] < 0) 
            continue;
        
            pcs.insert(playedNote[s]);
        
        if (bassPc < 0) 
            bassPc = playedNote[s];
    }

    if (pcs.empty())     
        return {};
    
    if (pcs.size() == 1) 
        return noteName[*pcs.begin()];

    // Paso 2: scoring por (raíz candidata, plantilla).
    //   matched : intervalos de la plantilla presentes en pcs
    //   missing : intervalos de la plantilla ausentes
    //   extra   : notas en pcs fuera de la plantilla
    //   score   = matched*3 - missing*4 - extra*1  (+1 si la raíz coincide con el bajo)
    QString bestName;
    int bestScore = std::numeric_limits<int>::min();
    int bestRoot  = -1;
    int bestSize  = 0;

    for (int root : pcs)
    {
        std::set<int> intervals;
        for (int pc : pcs)
            intervals.insert((pc - root + 12) % 12);

        for (const auto &t : templates)
        {
            int matched = 0;
            for (int i : t.iv)
                if (intervals.count(i)) ++matched;

            // Exigir al menos raíz + 3ra/5ta (2 intervalos) para evitar matches espurios.
            if (matched < 2) continue;

            const int tplSize = static_cast<int>(t.iv.size());
            const int missing = tplSize - matched;
            const int extra   = static_cast<int>(intervals.size()) - matched;

            int score = matched * 3 - missing * 4 - extra * 1;
            if (root == bassPc) 
                score += 1;

            if (score > bestScore || (score == bestScore && tplSize > bestSize))
            {
                bestScore = score;
                bestSize  = tplSize;
                bestRoot  = root;
                bestName  = QString("%1%2").arg(noteName[root]).arg(t.suffix);
            }
        }
    }

    if (bestName.isEmpty())
        return "?";

    // Inversión / slash chord: si la nota más grave no es la raíz.
    if (bestRoot >= 0 && bassPc >= 0 && bassPc != bestRoot)
        bestName += QString("/%1").arg(noteName[bassPc]);

    return bestName;
}
