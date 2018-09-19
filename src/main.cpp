#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QSplashScreen>
#include <QStyle>
#include <QTextStream>
#include <QFileInfo>
#include <cameraselectdialog.h>

#include <unistd.h>
#include <signal.h>

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
    QFile lockfile{"/tmp/.LiveView-lock"};
    QFileInfo lockfile_info{"/tmp/.LiveView-lock"};
    QTextStream lockfile_stream{&lockfile};
    if(lockfile_info.exists() || !lockfile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        auto reply = QMessageBox::question(nullptr, "LiveView Cannot Start",
                            "Only one instance of LiveView may be run at a time. Would you like the other LiveView instance to be stopped?",
                                           QMessageBox::Yes|QMessageBox::Cancel);
        if(reply == QMessageBox::Cancel) {
            return EXIT_FAILURE;
        } else {
            lockfile.open(QIODevice::ReadWrite | QIODevice::Text);
            QString line = lockfile_stream.readLine();
	    //Do some error checking here so it doesn't just die if the process isn't owned by the user
            auto pid = line.toInt();
            if(pid == 0 || kill(pid, SIGKILL) == -1) {
                QMessageBox::information(nullptr, "Cannot kill LiveView", "The currently open LiveView process cannot be killed. To resolve this, manually end the other LiveView process or restart your system. LiveView will now Close.", QMessageBox::Button::Abort);
                return -1;
            }
        }
    }

    int my_pid = getpid();
    qDebug() << "PID:" << my_pid;
    lockfile.close();
    lockfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    lockfile_stream << my_pid;
    lockfile.close();
    QSettings settings(QStandardPaths::writableLocation(
                           QStandardPaths::ConfigLocation)
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
        if (!retval) {
            lockfile.remove();
            return -1;
        }
    }

    QPixmap logo_pixmap(":images/aviris-logo-transparent.png");
    QSplashScreen splash(logo_pixmap);
    splash.show();
    splash.showMessage(QObject::tr("Loading LiveView... Compiled on " __DATE__ ", " __TIME__ " PDT by " UNAME "@" HOST),
                       Qt::AlignCenter | Qt::AlignBottom, Qt::gray);

    qDebug() << "This version (" << GIT_CURRENT_SHA1_SHORT << ") of LiveView was compiled on"
             << __DATE__ << "at" << __TIME__ << "using gcc" << __GNUC__;
    qDebug() << "The compilation was performed by" << UNAME << "@" << HOST;

    LVMainWindow w(&settings);
    w.setGeometry(QStyle::alignedRect(
                      Qt::LeftToRight,
                      Qt::AlignCenter,
                      w.size(),
                      a.desktop()->availableGeometry()));
    w.show();
    splash.finish(&w);

    auto ret_val = a.exec();
    lockfile.remove();
    return ret_val;
}
