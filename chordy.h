#pragma once

#include <QMainWindow>
#include <QPushButton>
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
    void onGridChanged(int8_t startFret,
                       std::array<ChordGrid::TopMarker, ChordGrid::totalStrings> markers,
                       std::vector<ChordGrid::Dot> dots,
                       int8_t barreFret,
                       int8_t barreFrom,
                       int8_t barreTo);

private:
    static constexpr int windowInitialWidth  = 320;
    static constexpr int windowInitialHeight = 580;
    static constexpr int windowMaxWidth      = 320;
    static constexpr int windowMaxHeight     = 580;

    QString detectChord(int startFret,
                        const std::array<ChordGrid::TopMarker, ChordGrid::totalStrings> &markers,
                        const std::vector<ChordGrid::Dot> &dots,
                        int barreFret,
                        int barreFrom,
                        int barreTo) const;

    ChordGrid *pGrid = nullptr;
    QPushButton *pDoFret = nullptr;
    QPushButton *pCopy   = nullptr;
};
