#include <iostream>

#include <QVBoxLayout>
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
        pGrid = new ChordGrid(this);
    }
    catch(const std::bad_alloc& e)
    {
        std::cout << e.what() << '\n';
        return false;
    }
    
    if(pGrid->create())
    {
        setCentralWidget(pGrid);

        // Ejemplo: Em9/11 desde traste VI
        // cuerdas: 0=Mi grave ... 5=Mi agudo
        // O=open, X=muted
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

        // barre parcial en fila 1, cuerdas 1 a 5
        pGrid->setChord("Em9/11", 6, markers, dots, 1, 1, 5);
    }

    return true;
}
