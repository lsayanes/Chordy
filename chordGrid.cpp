
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QLabel>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QToolButton>
#include <iostream>
#include <algorithm>
#include "chordGrid.h"

ChordGrid::ChordGrid(QWidget *parent, const uint8_t frets)
    : QWidget(parent), totalFrets { frets }
{
}

QToolButton *ChordGrid::createFretButton(const std::string &toolTip, Qt::ArrowType type)
{
    QToolButton *pBttn = new QToolButton(this);

    pBttn->setArrowType(type);
    pBttn->setAutoRepeat(true);
    pBttn->setAutoRepeatDelay(300);
    pBttn->setAutoRepeatInterval(80);
    pBttn->setFixedSize(26, 22);
    pBttn->setToolTip(toolTip.c_str());

	pBttn->setStyleSheet(
			"QToolButton {"
			"  background-color: rgb(200, 200, 200);"
			"  color: rgb(0, 0, 0);"
			"  border: 1px solid rgb(200, 200, 200);"
			"}");    
    
    return pBttn;
}

bool ChordGrid::create()
{
    m_fretUp = createFretButton("Traste inicial más agudo", Qt::UpArrow);
    connect(m_fretUp, &QToolButton::clicked, this, &ChordGrid::onStartFretUp);

    m_fretLabel = new QLabel(this);
    m_fretLabel->setAlignment(Qt::AlignCenter);
    {
        QFont f = m_fretLabel->font();
        f.setPixelSize(20);
        m_fretLabel->setFont(f);
    }
    m_fretLabel->setMinimumWidth(26);
    m_fretLabel->setFixedHeight(24);

    m_fretDown = createFretButton("Traste inicial más grave", Qt::DownArrow); 
    connect(m_fretDown, &QToolButton::clicked, this, &ChordGrid::onStartFretDown);

    updateFretLabelText();
    updateFretButtonsEnabled();
    positionFretControls();

    return true;
}

void ChordGrid::setChord(const std::string              &name,
                          int8_t                             startFret,
                          const std::array<TopMarker, totalStrings> &markers,
                          const std::vector<Dot>          &dots,
                          int8_t                             barreFret,
                          int8_t                             barreFrom,
                          int8_t                             barreTo)
{
    m_name      = name;
    m_startFret = startFret;
    m_markers   = markers;
    m_dots      = dots;
    m_barreFret = barreFret;
    m_barreFrom = barreFrom;
    m_barreTo   = barreTo;
    updateFretLabelText();
    updateFretButtonsEnabled();
    positionFretControls();
    update();
}

QString ChordGrid::toRoman(int8_t n)
{
    static const char *table[] = {
        "I","II","III","IV","V","VI","VII","VIII","IX","X",
        "XI","XII","XIII","XIV","XV", "XVI", "XVII", "XVIII", "XIX", "XX"
    };
    if (n >= 1 && n <= chordMaxFret)
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
    for (int8_t f = 0; f <= totalFrets; f++) 
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
    for (int8_t s = 0; s < totalStrings; s++) 
    {
        if (m_markers[s] == None) 
            continue;
        
        QString text = (m_markers[s] == Open) ? "O" : "X";
        int x = x0 + s * cellW;
        QRect markerRect(x - 8, 2, 16, top - 4);
        p.drawText(markerRect, Qt::AlignCenter, text);
    }

    // El número de traste (romano) lo muestra m_fretLabel, con botones ↑/↓ encima y debajo.

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
    int8_t s = (px - left + cellW / 2) / cellW;
    if (s < 0 || s >= totalStrings) 
        return;

    // determinar traste (1-based)
    int8_t f = (py - top) / cellH + 1;
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

    refresh();
}

void ChordGrid::setName(const QString &name)
{
    m_name = name.toStdString();
    update();
}

void ChordGrid::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    positionFretControls();
}


void ChordGrid::positionFretControls()
{
    if (!m_fretUp || !m_fretDown || !m_fretLabel)
        return;
    if (width() <= 0 || height() <= 0)
        return;

    const int x0    = left + 10;
    const int y0    = top;
    const int gridW = width()  - left - right;
    const int gridH = height() - top  - bottom;
    if (gridW <= 0 || gridH <= 0)
        return;

    const int cellW = gridW / (totalStrings - 1);
    const int cellH = gridH / totalFrets;

    // Siempre alineado a la primera fila del mástil; no mover con la cejilla (m_barreFret).
    const int cy = y0 + cellH / 2;
    const int x       = x0 + (totalStrings - 1) * cellW + 6;

    const int btnW = m_fretUp->width();
    const int btnH = m_fretUp->height();
    const int labelH = m_fretLabel->height();
    QFontMetrics fm(m_fretLabel->font());
    const int labelTextW = fm.horizontalAdvance(m_fretLabel->text());
    const int colW = qMax(btnW, labelTextW + 8);

    const int gap = 2;
    const int totalH = btnH + gap + labelH + gap + btnH;
    int topY = cy - totalH / 2;

    m_fretUp->setGeometry(x, topY, colW, btnH);
    m_fretLabel->setGeometry(x, topY + btnH + gap, colW, labelH);
    m_fretDown->setGeometry(x, topY + btnH + gap + labelH + gap, colW, btnH);
}

void ChordGrid::updateFretLabelText()
{
    if (!m_fretLabel)
        return;
    if (m_startFret > 0)
        m_fretLabel->setText(toRoman(m_startFret));
    else
        m_fretLabel->clear();
}

void ChordGrid::updateFretButtonsEnabled()
{
    if (m_fretUp)
        m_fretUp->setEnabled(m_startFret < chordMaxFret);
    if (m_fretDown)
        m_fretDown->setEnabled(m_startFret > 1);
}

void ChordGrid::onStartFretUp()
{
    if (m_startFret >= chordMaxFret)
        return;
    
    m_startFret++;

    refreshFret();
    refresh();
}

void ChordGrid::onStartFretDown()
{
    if (m_startFret <= 1)
        return;

    m_startFret--;

    refreshFret();
    refresh();
}

void ChordGrid::doFret()
{

    if(m_barreFret > 0)
    {
        m_barreFret = -1;
    }
    else 
    {
        size_t total = m_dots.size();

        if (total > 1)
        {    

            const auto &last = m_dots[total - 1];
            const auto &second_last = m_dots[total - 2];

            if(
                last.fret == second_last.fret &&
                last.string != second_last.string)
            {         
                m_barreFret = last.fret;
                m_barreFrom = last.string;
                m_barreTo = second_last.string;

                std::vector<Dot>::iterator it = m_dots.begin();

                while (it != m_dots.end())
                {
                    if(it->fret == m_barreFret)
                        it = m_dots.erase(it);
                    else
                        it++;
                }
            }

        }
    }

    refresh();
}

void ChordGrid::refresh()
{
    update();
    emit gridChanged(m_startFret, m_markers, m_dots, m_barreFret, m_barreFrom, m_barreTo);
}

void ChordGrid::refreshFret()
{
    updateFretLabelText();
    updateFretButtonsEnabled();
    positionFretControls();

}
