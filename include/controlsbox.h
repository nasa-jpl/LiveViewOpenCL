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

    /* This button shares functionality with the "Save As..." functionality
     * in the Main Window. It is a public member so that it can be connected
     * to the same "Save As..." function in the parent.
     */
    QPushButton *browseButton;
    QLineEdit *saveFileNameEdit;
    org_t bit_org;

public slots:
    void collectDSFMask();
    void tabChanged(int);
    void acceptSave();

private slots:
    void updateFPS(double frameRate);
    void setPrecision(bool isPrecise);
    void setMinSpin(int new_min);
    void setMaxSpin(int new_max);
    void setRangeSliderMin(int new_min);
    void setRangeSliderMax(int new_max);

private:
    int fileNumber = 0;
    QString prevFileName;

    FrameWorker *frame_handler;
    QLabel *fpsLabel;

    QTabWidget* tab_handler;

    QString findAndReplaceFileName(const QString &fileName);
    LVTabApplication* getCurrentTab();
    LVTabApplication *viewWidget;

    ctkRangeSlider *rangeSlider;
    QCheckBox *precisionBox;
    QSpinBox *min_box;
    QSpinBox *max_box;


    QSpinBox *numFramesEdit;
    QSpinBox *numAvgsEdit;

    QPushButton *maskButton;
    bool collecting_mask;
};

#endif // CONTROLSBOX_H
