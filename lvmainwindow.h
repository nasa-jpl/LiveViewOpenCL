#ifndef LVMAINWINDOW_H
#define LVMAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QVBoxLayout>
#include <QDebug>

#include "image_type.h"
#include "frameworker.h"
#include "framethread.h"
#include "frameview_widget.h"
#include "controlsbox.h"

class LVMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LVMainWindow(QWidget *parent = nullptr);
    ~LVMainWindow();

public slots:
    void errorString(const QString &);

private:
    FrameWorker* fw;
    frameview_widget* raw_display;
    ControlsBox* cbox;
};

#endif // LVMAINWINDOW_H
