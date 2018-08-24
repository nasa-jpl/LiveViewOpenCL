#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QSplashScreen>
#include <QStyle>
#include <QTextStream>
#include <cameraselectdialog.h>

#include "lvmainwindow.h"

#ifndef HOST
#define HOST "unknown location"
#endif

#ifndef UNAME
#define UNAME "unknown person"
#endif

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QSettings settings(QStandardPaths::writableLocation(
                           QStandardPaths::AppConfigLocation)
                       + "/lvconfig.ini", QSettings::IniFormat);

    if (settings.value(QString("dark"), USE_DARK_STYLE).toBool()) {
        QFile f(":qdarkstyle/style.qss");

        if (!f.exists()) {
            printf("Unable to set stylesheet, file not found\n");
        } else {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
        }
    }

    CameraSelectDialog csd(&settings);
    if (settings.value(QString("show_cam_dialog"), true).toBool()) {
        int retval = csd.exec();
        if (!retval) return -1;
    }

    QPixmap logo_pixmap(":images/aviris-logo-transparent.png");
    QSplashScreen splash(logo_pixmap);
    splash.show();
    splash.showMessage(QObject::tr("Loading LiveView... Compiled on " __DATE__ ", " __TIME__ " PDT by " UNAME "@" HOST), Qt::AlignCenter | Qt::AlignBottom, Qt::gray);

    qDebug() << "This version of LiveView was compiled on" << __DATE__ << "at" << __TIME__ << "using gcc" << __GNUC__;
    qDebug() << "The compilation was performed by" << UNAME << "@" << HOST << "\n";


    LVMainWindow w(&settings);
    w.setGeometry(QStyle::alignedRect(
                      Qt::LeftToRight,
                      Qt::AlignCenter,
                      w.size(),
                      a.desktop()->availableGeometry()));
    w.show();
    splash.finish(&w);

    return a.exec();
}
