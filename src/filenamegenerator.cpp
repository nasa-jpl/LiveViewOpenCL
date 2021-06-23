#include "filenamegenerator.h"

fileNameGenerator::fileNameGenerator()
{
    shortFilename = "unset";
    directory = "unset";
    extension = "unset";
}

void fileNameGenerator::setMainDirectory(QString basedir)
{
    this->directory = basedir;
}

void fileNameGenerator::setFilenameExtension(QString extension)
{
    this->extension = extension;
}

QString fileNameGenerator::getNewFullFilename(QString basedir, QString extension)
{
    this->directory = basedir;
    this->extension = extension;
    generate();
    return getFullFilename();
}

QString fileNameGenerator::getNewFullFilename()
{
    generate();
    return getFullFilename();
}

QString fileNameGenerator::getFullFilename()
{
    return directory + "/" + shortFilename + "." + extension;
}

QString fileNameGenerator::getShortFilename()
{
    return shortFilename + extension;
}

QString fileNameGenerator::getFilenameExtension()
{
    return extension;
}

void fileNameGenerator::generate()
{
    shortFilename = getTimeDatestring();
}

QString fileNameGenerator::getTimeDatestring()
{
    // YYYY-MM-DD_HHMMSS UTC
    QDateTime now = QDateTime::currentDateTimeUtc();
    QString dateString = now.toString("yyyy-MM-dd_hhmmss");

    qDebug() << __PRETTY_FUNCTION__ << "time string: " << dateString;
    return dateString;
}

