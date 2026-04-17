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

private slots:
    void onGridChanged(int startFret,
                       std::array<ChordGrid::TopMarker, ChordGrid::totalStrings> markers,
                       std::vector<ChordGrid::Dot> dots);

private:
    QString detectChord(int startFret,
                        const std::array<ChordGrid::TopMarker, ChordGrid::totalStrings> &markers,
                        const std::vector<ChordGrid::Dot> &dots) const;

    ChordGrid *pGrid = nullptr;
};
