#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QSplashScreen>
#include <QStyle>
#include <QTextStream>
#include <QFileInfo>
#include <cameraselectdialog.h>

#include <QMutex>     // PK 2-17-21 image-line-debug

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <csignal>

#include <string.h>

#include "lvmainwindow.h"

#ifndef HOST
#define HOST "unknown location"
#endif

#ifndef UNAME
#define UNAME "unknown person"
#endif

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)


bool frameLineDebugLog = false;

// this mutex is for loading and retrieving image frame line
// data from the shared frame data list
QMutex frameDataMutex;  // PK 2-17-21 image-line-debug

// this mutex is for storing and retrieving image frame line
// data for LiveView display
QMutex LVDataMutex;  // PK 2-18-21 image-line-debug

QMutex frameLineDataMutex;  // PK 3-8-21 image-line-debug



int main(int argc, char* argv[])
{
    int sfd;
    struct sockaddr_un lv_addr = {};

    if( argc > 1 )
        frameLineDebugLog = strcmp(argv[1], "-lineDebug") ? false : true;

    // The QApplication class manages the GUI application's control flow
    // and main settings
    QApplication Qt_App(argc, argv);

    std::string socket_path = "/tmp/LiveViewOpenSource";

    if ( (sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        handle_error("socket");
        return EXIT_FAILURE;
    }

    memset(&lv_addr, 0, sizeof(lv_addr));
    lv_addr.sun_family = AF_UNIX;
    strncpy(lv_addr.sun_path, socket_path.data(), sizeof(lv_addr.sun_path) - 1);

    strncpy(lv_addr.sun_path, socket_path.data(), sizeof(lv_addr.sun_path) - 1);

    if (bind(sfd, reinterpret_cast<struct sockaddr*>(&lv_addr), sizeof(lv_addr)) == -1) {
        auto reply = QMessageBox::question(nullptr, "LiveView Cannot Start",
                            "Only one instance of LiveView should be run at a time. Multiple instances can cause errors. Would you like to continue anyways?",
                                           QMessageBox::Yes | QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) {
            handle_error("bind");
        } else {
            unlink(socket_path.data());
            bind(sfd, reinterpret_cast<struct sockaddr*>(&lv_addr), sizeof(struct sockaddr_un));
        }
    }

    if (listen(sfd, 50) == -1) {
        qDebug() << strerror(errno);
        handle_error("listen");
    }

    /*
     * Initialize LiveView application ... with its settings 
     * defined in ~/.config/lvconfig.ini
     */
    QSettings settings(QStandardPaths::writableLocation(
                           QStandardPaths::ConfigLocation)
                       + "/lvconfig.ini", QSettings::IniFormat);

    /*
     * Load LiveView configuration and settings from its configuration 
     * file ~/.config/lvconfig.ini
     */
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
            unlink(socket_path.data());
            return -1;
        }
    }


    QPixmap logo_pixmap(":images/aviris-logo-transparent.png");
    QSplashScreen splash(logo_pixmap); 
    /* splash.show(); PK 11-11-20 temp. disabled for debug */
    splash.showMessage(QObject::tr("Loading LiveView... Compiled on " __DATE__ ", " __TIME__ " PDT by " UNAME "@" HOST),
                       Qt::AlignCenter | Qt::AlignBottom, Qt::gray);

    qDebug() << "This version (" << GIT_CURRENT_SHA1_SHORT << ") of LiveView was compiled on"
             << __DATE__ << "at" << __TIME__ << "using gcc" << __GNUC__;
    qDebug() << "The compilation was performed by" << UNAME << "@" << HOST;


    //
    // Initializes LiveView Main Window, set up slots (event handlers),
    // create Actions, set up QFuture watchers, starting threads for
    // processing SD frames.
    LVMainWindow w(&settings);
    if (w.notInitialized) {
        splash.finish(&w);
        auto ret_val = 1;
        unlink(socket_path.data());
        return ret_val;
    }
    w.setGeometry(QStyle::alignedRect(
                      Qt::LeftToRight,
                      Qt::AlignCenter,
                      w.size(),
                      Qt_App.desktop()->availableGeometry()));
    w.show();
    splash.finish(&w);


    // For any GUI application using Qt, there is precisely one
    // QApplication object, no matter whether the application
    // has 0, 1, 2 or more windows at any given time.
    //
    // Enters the main event loop and waits until exit() is called,
    // then returns the value that was set to exit() (which is 0
    // if exit() is called via quit()).
    //
    // It is necessary to call this function 'exec()' to start event
    // handling. The main event loop receives events from the window
    // system and dispatches these to the application widgets.
    auto ret_val = Qt_App.exec();

    //
    // LiveView application exits, cleans up and remove the data from
    // the file system.
    unlink(socket_path.data());

    return ret_val;

} // end of main()
