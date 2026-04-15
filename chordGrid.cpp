
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QMouseEvent>
#include <algorithm>
#include "chordGrid.h"

ChordGrid::ChordGrid(QWidget *parent, const uint8_t frets)
    : QWidget(parent), totalFrets { frets }
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

    const int x0    = left;
    const int y0    = top;
    const int gridW = width()  - left - right;
    const int gridH = height() - top  - bottom;
    const int cellW = gridW / (totalStrings - 1);
    const int cellH = gridH / totalFrets;

    const int string_top    = top;
    const int string_bottom = top + totalFrets * cellH;
    const int neck_width    = left + (totalStrings - 1) * cellW;

    // Nut 
    if (1 == m_startFret) 
    {
        p.setPen(QPen(Qt::black, 5));
        p.drawLine(x0, y0, neck_width, y0);
    }

    p.setPen(QPen(Qt::black, 1));
    //strings
    for (int s = 0; s < totalStrings; ++s) 
    {
        int x = x0 + s * cellW;
        p.drawLine(x, string_top, x, string_bottom);
    }

    //trastes
    for (int f = 0; f <= totalFrets; f++) 
    {
        if (f == 0 && m_startFret == 1) 
            continue; 
        
        int y = y0 + f * cellH;
        p.drawLine(x0, y, neck_width, y);
    }

    // Marcadores O y X
    QFont font = p.font();
    font.setPixelSize(14);
    p.setFont(font);
    p.setPen(Qt::black);
    for (int s = 0; s < totalStrings; s++) 
    {
        if (m_markers[s] == None) continue;
        QString text = (m_markers[s] == Open) ? "O" : "X";
        int x = x0 + s * cellW;
        QRect markerRect(x - 8, 2, 16, top - 4);
        p.drawText(markerRect, Qt::AlignCenter, text);
    }

    // Número cejilla
    if (m_startFret > 1) 
    {
        int fretRow = (m_barreFret > 0) ? m_barreFret : 1;
        int y = y0 + (fretRow - 1) * cellH + cellH / 2;
        int x = x0 + (totalStrings - 1) * cellW + 10;
        font.setPixelSize(12);
        p.setFont(font);
        QRect romanRect(x, y - 10, right - 8, 20);
        p.drawText(romanRect, Qt::AlignVCenter | Qt::AlignLeft, toRoman(m_startFret));
    }

    //cejilla
    if (m_barreFret > 0) 
    {
        int y  = y0 + (m_barreFret - 1) * cellH + cellH / 2;
        int x1 = x0 + m_barreFrom * cellW;
        int x2 = x0 + m_barreTo   * cellW;
        int thickness = static_cast<int>(cellH * 0.05);
        p.setPen(QPen(Qt::black, thickness, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(x1, y, x2, y);
    }

    // dedos 
    p.setBrush(Qt::black);
    p.setPen(Qt::NoPen);
    int dotR = static_cast<int>(cellW * 0.3);
    for (const auto &dot : m_dots) 
    {
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
        QRect nameRect(x0, y0 + totalFrets * cellH + 4, (totalStrings - 1) * cellW, bottom);
        p.drawText(nameRect, Qt::AlignHCenter | Qt::AlignTop, QString::fromStdString(m_name));
    }
}

void ChordGrid::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    const int gridW = width()  - left - right;
    const int gridH = height() - top  - bottom;
    const int cellW = gridW / (totalStrings - 1);
    const int cellH = gridH / totalFrets;

    const int px = event->pos().x();
    const int py = event->pos().y();

    // determinar cuerda más cercana
    int s = (px - left + cellW / 2) / cellW;
    if (s < 0 || s >= totalStrings) 
        return;

    // determinar traste (1-based)
    int f = (py - top) / cellH + 1;
    if (f < 1 || f > totalFrets) 
        return;

    // toggle: si ya existe ese dot, lo remueve; si no, lo agrega
    auto it = std::find_if(m_dots.begin(), m_dots.end(),[s, f](const Dot &d) 
    { 
        return d.string == s && d.fret == f; 
    });

    if (it != m_dots.end())
        m_dots.erase(it);
    else
        m_dots.push_back({s, f});

    update();
    emit gridChanged(m_startFret, m_markers, m_dots);
}

void ChordGrid::setName(const QString &name)
{
    m_name = name.toStdString();
    update();
}
