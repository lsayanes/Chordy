#pragma once

#include <QMainWindow>
#include <QWidget>

#include "chordGrid.h"


class Chordy : public QMainWindow
{
    Q_OBJECT

public:
    explicit Chordy(QWidget *parent = nullptr);
    virtual ~Chordy() = default;

    bool create(const std::string &title = "Chordy");
private:

    ChordGrid *pGrid;
};
