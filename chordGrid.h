
#pragma once

#include <QWidget>
#include <QString>
#include <array>
#include <vector>
#include <string>

class ChordGrid : public QWidget
{
public:
    enum TopMarker { None, Open, Muted };

    struct Dot {
        int string; // 0 = low E ... 5 = high E
        int fret;   // 1-based relativo al diagrama
    };

    explicit ChordGrid(QWidget *parent);
    ~ChordGrid() override = default;

    bool create();

    void setChord(const std::string              &name,
                  int                             startFret,
                  const std::array<TopMarker, 6> &markers,
                  const std::vector<Dot>          &dots,
                  int                             barreFret = -1,
                  int                             barreFrom = 0,
                  int                             barreTo   = 5);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static QString toRoman(int n);

    std::string              m_name;
    int                      m_startFret = 1;
    int                      m_barreFret = -1;  // fila 1-based, -1 = sin barre
    int                      m_barreFrom = 0;
    int                      m_barreTo   = 5;
    std::array<TopMarker, 6> m_markers   = {};
    std::vector<Dot>         m_dots;
};
