#ifndef CONTROLSBOX_H
#define CONTROLSBOX_H

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QByteArray>
#include <QTabWidget>

#include "frameworker.h"
#include "lvtabapplication.h"
#include "ctkrangeslider.h"

class ControlsBox : public QWidget
{
    Q_OBJECT
public:
    explicit ControlsBox(FrameWorker *fw, QTabWidget *tw,
                         QString ipAddress, quint16 port, QWidget *parent = nullptr);
    ~ControlsBox();

public slots:
    void collectDSFMask();
    void tabChanged(int);

private slots:
    void updateFPS(float frameRate);
    void setPrecision(bool isPrecise);
    void setMinSpin(int new_min);
    void setMaxSpin(int new_max);
    void setRangeSliderMin(int new_min);
    void setRangeSliderMax(int new_max);

private:
    FrameWorker *frame_handler;
    QLabel *fpsLabel;

    QTabWidget* tab_handler;

    LVTabApplication* getCurrentTab();
    LVTabApplication *viewWidget;

    ctkRangeSlider *rangeSlider;
    QCheckBox *precisionBox;
    QSpinBox *min_box;
    QSpinBox *max_box;

    QPushButton *maskButton;
    bool collecting_mask;
};

#endif // CONTROLSBOX_H
