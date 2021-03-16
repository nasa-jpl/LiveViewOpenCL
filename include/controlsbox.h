#ifndef CONTROLSBOX_H
#define CONTROLSBOX_H

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QByteArray>
#include <QTabWidget>

#include "frameworker.h"
#include "frameview_widget.h"   // PK 3-4-21 image-line-control
#include "lvtabapplication.h"
#include "ctkrangeslider.h"

class ControlsBox : public QWidget
{
    Q_OBJECT
public:
    explicit ControlsBox(FrameWorker *fw, QTabWidget *tw,
                         const QString &ipAddress, quint16 port, QWidget *parent = nullptr);
    ~ControlsBox() = default;

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

    // 
    // PK EMITFPIED-331_v2 Frame Control enhancement
    //
    // add Frame Control buttons: Rewind, Play/Stop, Forward
    void frameControlPrevButtonClicked();
    void frameControlNextButtonClicked();
    void frameControlStopButtonClicked();

    // 3/3/21 line-control-debug ...
    void frameLineControlChecked( bool enabled );
    void updateFrameFilename( const std::string filename );
    void updateFrameLineInfo();     // 3-10-21 image-line-control


private:
    int fileNumber = 0;
    QString prevFileName;

    FrameWorker *frame_handler;
    QLabel *fpsLabel;

    QTabWidget *tab_handler;

    QString findAndReplaceFileName(const QString &fileName);
    LVTabApplication *getCurrentTab();
    LVTabApplication *viewWidget;

    ctkRangeSlider *rangeSlider;
    QCheckBox *precisionBox;
    QSpinBox *min_box;
    QSpinBox *max_box;


    QSpinBox *numFramesEdit;
    QSpinBox *numAvgsEdit;

    QPushButton *maskButton;
    bool collecting_mask;

    //
    // EMITFPIED-331
    // need to coummnicate with FrameWorker thread
    FrameWorker *frameWorkerParent;

    //
    // PK EMITFPIED-331_v2 Frame Control enhancement
    // 
    // add Frame Control buttons: Rewind, Play/Stop, Forward
    QToolBar    *frameControlBar;
    QToolButton *frameControl_prevButton;
    QToolButton *frameControl_nextButton;
    QToolButton *frameControl_stopButton;

    //
    // 3-3-21 Frame line control 
    frameview_widget *frameDisplay;

    QLabel    *frameControlStatus;
    QLabel    *frameLineControlStatus;

    QLabel    *frameFilename;

    //
    // 3-10-21 attempt to fix widget update latency issue 
    QLabel    *frameLineNo;
    QLabel    *frameLineCount;
    QLabel    *frameCollectionID;

    QCheckBox   *frameLineControl;   // 3-3-21 line-control-debug

};

#endif // CONTROLSBOX_H
