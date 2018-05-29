#ifndef LVMAINWINDOW_H
#define LVMAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QThread>
#include <QVBoxLayout>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include "image_type.h"
#include "frameview_widget.h"
#include "histogram_widget.h"
#include "line_widget.h"
#include "controlsbox.h"

class LVMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LVMainWindow(QWidget *parent = NULL);
    ~LVMainWindow();

public slots:
    void errorString(const QString &);

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

private:
    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;

    FrameWorker* fw;
    QFuture<void> DSLoop;
    QFuture<void> SDLoop;
    QTabWidget* tab_widget;
    frameview_widget* raw_display;
    frameview_widget* dsf_display;
    frameview_widget* sdv_display;
    histogram_widget* hst_display;
    line_widget* spec_display;
    line_widget* spat_display;
    ControlsBox* cbox;

    QString default_dir;
    QString save_filename;

private slots:
    void open();
    void save();
    void saveAs();
};

#endif // LVMAINWINDOW_H
