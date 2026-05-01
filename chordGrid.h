
#pragma once

#include <QWidget>
#include <QString>
#include <array>
#include <vector>
#include <string>
#include <QToolButton>
#include <QLabel>
#include <QResizeEvent>


class ChordGrid : public QWidget
{
    Q_OBJECT

public:

    static constexpr uint8_t totalStrings { 6 };
    static constexpr uint8_t chordMaxFret { 20 };

    static constexpr int top      { 40 };
    static constexpr int left     { 20 };
    static constexpr int right    { 65 };
    static constexpr int bottom   { 50 };


    enum TopMarker { None, Open, Muted };

    struct Dot {
        int8_t string; // 0 = low E ... 5 = high E
        int8_t fret;   // 1-based relativo al diagrama
    };

    explicit ChordGrid(QWidget *parent, const uint8_t frets);
    ~ChordGrid() override = default;

    bool create();

    void setChord(const std::string                             &name,
                  int8_t                                           startFret,
                  const std::array<TopMarker, totalStrings>     &markers,
                  const std::vector<Dot>                        &dots,
                  int8_t                             barreFret = -1,
                  int8_t                             barreFrom = 0,
                  int8_t                             barreTo   = 5);

    void setName(const QString &name);

    void refresh();
    void refreshFret();

private slots:
    void onStartFretUp();
    void onStartFretDown();
public slots:
	void doFret();

signals:
    void gridChanged(int8_t startFret,
                     std::array<ChordGrid::TopMarker, totalStrings> markers,
                     std::vector<ChordGrid::Dot> dots);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    static QString toRoman(int8_t n);
    void positionFretControls();
    void updateFretLabelText();
    void updateFretButtonsEnabled();

    QToolButton *createFretButton(const std::string &toolTip, Qt::ArrowType type);

    QToolButton *m_fretUp   = nullptr;
    QToolButton *m_fretDown = nullptr;
    QLabel      *m_fretLabel = nullptr;

    std::string              m_name;
    int8_t                      m_startFret = 1;
    int8_t                      m_barreFret = -1;  // fila 1-based, -1 = sin barre
    int8_t                      m_barreFrom = 0;
    int8_t                      m_barreTo   = 5;

    std::array<TopMarker, totalStrings>  m_markers   = {}; //O u X
    std::vector<Dot>                     m_dots; //dedos

    const uint8_t totalFrets;


};
