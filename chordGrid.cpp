
#include <QPainter>
#include <QPen>
#include <QFont>
#include "chordGrid.h"

ChordGrid::ChordGrid(QWidget *parent)
    : QWidget(parent)
{
}

bool ChordGrid::create()
{
    return true;
}

void ChordGrid::setChord(const std::string              &name,
                          int                             startFret,
                          const std::array<TopMarker, 6> &markers,
                          const std::vector<Dot>          &dots,
                          int                             barreFret,
                          int                             barreFrom,
                          int                             barreTo)
{
    m_name      = name;
    m_startFret = startFret;
    m_markers   = markers;
    m_dots      = dots;
    m_barreFret = barreFret;
    m_barreFrom = barreFrom;
    m_barreTo   = barreTo;
    update();
}

QString ChordGrid::toRoman(int n)
{
    static const char *table[] = {
        "I","II","III","IV","V","VI","VII","VIII","IX","X",
        "XI","XII","XIII","XIV","XV"
    };
    if (n >= 1 && n <= 15)
        return table[n - 1];
    return QString::number(n);
}

void ChordGrid::paintEvent(QPaintEvent * /* event */)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int strings = 6;
    const int frets   = 5;

    const int mTop    = 28;
    const int mRight  = 38;
    const int mBottom = 30;
    const int mLeft   = 12;

    const int x0    = mLeft;
    const int y0    = mTop;
    const int gridW = width()  - mLeft - mRight;
    const int gridH = height() - mTop  - mBottom;
    const int cellW = gridW / (strings - 1);
    const int cellH = gridH / frets;

    // Nut 
    if (m_startFret == 1) {
        p.setPen(QPen(Qt::black, 5));
        p.drawLine(x0, y0, x0 + (strings - 1) * cellW, y0);
    }

    //  Cuadricula 
    p.setPen(QPen(Qt::black, 1));
    for (int s = 0; s < strings; ++s) 
    {
        int x = x0 + s * cellW;
        p.drawLine(x, y0, x, y0 + frets * cellH);
    }
    
    for (int f = 0; f <= frets; ++f) 
    {
        if (f == 0 && m_startFret == 1) 
            continue; // ya dibujamos el nut
        
        int y = y0 + f * cellH;
        p.drawLine(x0, y, x0 + (strings - 1) * cellW, y);
    }

    // Marcadores O y X
    QFont font = p.font();
    font.setPixelSize(14);
    p.setFont(font);
    p.setPen(Qt::black);
    for (int s = 0; s < strings; ++s) {
        if (m_markers[s] == None) continue;
        QString text = (m_markers[s] == Open) ? "O" : "X";
        int x = x0 + s * cellW;
        QRect markerRect(x - 8, 2, 16, mTop - 4);
        p.drawText(markerRect, Qt::AlignCenter, text);
    }

    // Número cejilla
    if (m_startFret > 1) {
        int fretRow = (m_barreFret > 0) ? m_barreFret : 1;
        int y = y0 + (fretRow - 1) * cellH + cellH / 2;
        int x = x0 + (strings - 1) * cellW + 6;
        font.setPixelSize(12);
        p.setFont(font);
        QRect romanRect(x, y - 10, mRight - 8, 20);
        p.drawText(romanRect, Qt::AlignVCenter | Qt::AlignLeft, toRoman(m_startFret));
    }

    //cejilla
    if (m_barreFret > 0) {
        int y  = y0 + (m_barreFret - 1) * cellH + cellH / 2;
        int x1 = x0 + m_barreFrom * cellW;
        int x2 = x0 + m_barreTo   * cellW;
        int thickness = static_cast<int>(cellH * 0.55);
        p.setPen(QPen(Qt::black, thickness, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(x1, y, x2, y);
    }

    // dedos 
    p.setBrush(Qt::black);
    p.setPen(Qt::NoPen);
    int dotR = static_cast<int>(cellW * 0.3);
    for (const auto &dot : m_dots) {
        int x = x0 + dot.string * cellW;
        int y = y0 + (dot.fret - 1) * cellH + cellH / 2;
        p.drawEllipse(QPoint(x, y), dotR, dotR);
    }

    // Nombre del acorde
    if (!m_name.empty()) 
    {
        font.setPixelSize(14);
        font.setBold(true);
        p.setFont(font);
        p.setPen(Qt::black);
        QRect nameRect(x0, y0 + frets * cellH + 4, (strings - 1) * cellW, mBottom);
        p.drawText(nameRect, Qt::AlignHCenter | Qt::AlignTop, QString::fromStdString(m_name));
    }
}
