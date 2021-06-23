#ifndef FILENAMEGENERATOR_H
#define FILENAMEGENERATOR_H
#include <QString>
#include <QDateTime>

#include <QDebug>

class fileNameGenerator
{
public:
    fileNameGenerator();
    void setMainDirectory(QString basedir);
    void setFilenameExtension(QString extension);
    void generate(); // call this for new time stamp
    QString getNewFullFilename(QString basedir, QString extension); // auto new filename
    QString getNewFullFilename(); // auto new filename
    QString getFullFilename(); // use existing timestamp
    QString getShortFilename(); // use existing timestamp
    QString getFilenameExtension();

private:
    QString getTimeDatestring();
    QString shortFilename;
    QString directory;
    QString extension;
};

#endif // FILENAMEGENERATOR_H
