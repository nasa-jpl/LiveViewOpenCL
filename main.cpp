#include <QApplication>
#include <QDesktopWidget>
#include <QPixmap>
#include <QSplashScreen>
#include <QStyle>
#include <QTextStream>

#include "lvmainwindow.h"
#include "qcustomplot.h"
#include "frameworker.h"

#ifndef HOST
#define HOST "unknown location"
#endif

#ifndef UNAME
#define UNAME "unknown person"
#endif

// place in global namespace
QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QPixmap logo_pixmap(":images/aviris-logo-transparent.png");
    QSplashScreen splash(logo_pixmap);
    splash.show();
    splash.showMessage(QObject::tr("Loading AVIRIS-Next Generation LiveView. Compiled on " __DATE__ ", " __TIME__ " PDT by " UNAME "@" HOST), Qt::AlignCenter | Qt::AlignBottom, Qt::gray);

    qDebug() << "This version of LiveView was compiled on " << __DATE__ << " at " << __TIME__ << " using gcc " << __GNUC__;
    qDebug() << "The compilation was performed by " << UNAME << " @ " << HOST << "\n";
    LVMainWindow w;
    w.setGeometry(   QStyle::alignedRect(
                             Qt::LeftToRight,
                             Qt::AlignCenter,
                             w.size(),
                             a.desktop()->availableGeometry()
                             ));
    QPixmap icon_pixmap(":images/icon.png");
    w.setWindowIcon(QIcon(icon_pixmap));
    w.show();
    splash.finish(&w);

    return a.exec();

}
