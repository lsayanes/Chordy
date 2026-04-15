
#pragma once

#include <QWidget>
#include <QString>
#include <array>
#include <vector>
#include <string>

class QToolButton;
class QLabel;
class QResizeEvent;

class ChordGrid : public QWidget
{
    Q_OBJECT

public:

    static constexpr uint8_t totalStrings { 6 };
    static constexpr uint8_t chordMaxFret { 20 };

    enum TopMarker { None, Open, Muted };

    struct Dot {
        int string; // 0 = low E ... 5 = high E
        int fret;   // 1-based relativo al diagrama
    };

    explicit ChordGrid(QWidget *parent, const uint8_t frets);
    ~ChordGrid() override = default;

    bool create();

    void setChord(const std::string                             &name,
                  int                                           startFret,
                  const std::array<TopMarker, totalStrings>     &markers,
                  const std::vector<Dot>                        &dots,
                  int                             barreFret = -1,
                  int                             barreFrom = 0,
                  int                             barreTo   = 5);

    void setName(const QString &name);

private slots:
    void onStartFretUp();
    void onStartFretDown();

signals:
    void gridChanged(int startFret,
                     std::array<ChordGrid::TopMarker, totalStrings> markers,
                     std::vector<ChordGrid::Dot> dots);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    static QString toRoman(int n);
    void positionFretControls();
    void updateFretLabelText();
    void updateFretButtonsEnabled();

    QToolButton *m_fretUp   = nullptr;
    QToolButton *m_fretDown = nullptr;
    QLabel      *m_fretLabel = nullptr;

    std::string              m_name;
    int                      m_startFret = 1;
    int                      m_barreFret = -1;  // fila 1-based, -1 = sin barre
    int                      m_barreFrom = 0;
    int                      m_barreTo   = 5;

    std::array<TopMarker, totalStrings>  m_markers   = {}; //O u X
    std::vector<Dot>                     m_dots; //dedos

    const uint8_t totalFrets;

    static constexpr int top      { 28 };
    static constexpr int left     { 20 };
    static constexpr int right    { 65 };
    static constexpr int bottom   { 30 };

};
