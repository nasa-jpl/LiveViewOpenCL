#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QSplashScreen>
#include <QStyle>
#include <QTextStream>
#include <QFileInfo>
#include <cameraselectdialog.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <csignal>

#include "lvmainwindow.h"

#ifndef HOST
#define HOST "unknown location"
#endif

#ifndef UNAME
#define UNAME "unknown person"
#endif

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main(int argc, char* argv[])
{
    int sfd;
    struct sockaddr_un lv_addr = {};
    QApplication a(argc, argv);
    char* socket_path = "/tmp/LiveViewOpenSource";

    if ( (sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        handle_error("socket");
        return EXIT_FAILURE;
    }

    memset(&lv_addr, 0, sizeof(lv_addr));
    lv_addr.sun_family = AF_UNIX;
    strncpy(lv_addr.sun_path, socket_path, sizeof(lv_addr.sun_path) - 1);

    strncpy(lv_addr.sun_path, socket_path, sizeof(lv_addr.sun_path) - 1);

    if (bind(sfd, reinterpret_cast<struct sockaddr*>(&lv_addr), sizeof(lv_addr)) == -1) {
        auto reply = QMessageBox::question(nullptr, "LiveView Cannot Start",
                            "Only one instance of LiveView should be run at a time. Multiple instances can cause errors. Would you like to continue anyways?",
                                           QMessageBox::Yes | QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) {
            handle_error("bind");
        } else {
            unlink(socket_path);
            bind(sfd, reinterpret_cast<struct sockaddr*>(&lv_addr), sizeof(struct sockaddr_un));
        }
    }

    if (listen(sfd, 50) == -1) {
        qDebug() << strerror(errno);
        handle_error("listen");
    }

    QSettings settings(QStandardPaths::writableLocation(
                           QStandardPaths::ConfigLocation)
                       + "/lvconfig.ini", QSettings::IniFormat);

    if (settings.value(QString("dark"), USE_DARK_STYLE).toBool()) {
        QFile f(":qdarkstyle/style.qss");

        if (!f.exists()) {
            qDebug() << "Unable to set stylesheet, file not found";
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
            unlink(socket_path);
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
    unlink(socket_path);

    return ret_val;
}
