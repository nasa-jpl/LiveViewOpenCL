#ifndef LVMAINWINDOW_H
#define LVMAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QVBoxLayout>
#include <QDebug>

#include "image_type.h"
#include "framethread.h"
#include "frameview_widget.h"
#include "histogram_widget.h"
#include "controlsbox.h"

class LVMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LVMainWindow(QWidget *parent = NULL);
    ~LVMainWindow();

public slots:
    void errorString(const QString &);

private:
    FrameWorker* fw;
    QTabWidget* tab_widget;
    frameview_widget* raw_display;
    frameview_widget* dsf_display;
    frameview_widget* sdv_display;
    histogram_widget* hst_display;
    ControlsBox* cbox;
};

#endif // LVMAINWINDOW_H
