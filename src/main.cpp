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
    //QString version_str = GIT_CURRENT_SHA1;

    /*
        update_status_text("started program.");
        update_status_text(qobject::tr("source code compiled on " __date__ ", " __time__ " pdt by " uname "@" host  ));
        version = git_current_sha1;
        version.prepend("git commit sha1: ");
        update_status_text(version);
        version = git_current_sha1_short;
        version.prepend("git commit sha1 short: ");
        update_status_text(version);
        */

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

    qDebug() << "This version (" << GIT_CURRENT_SHA1_SHORT << ") of LiveView was compiled on" << __DATE__ << "at" << __TIME__ << "using gcc" << __GNUC__;
    qDebug() << "The compilation was performed by" << UNAME << "@" << HOST;

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
