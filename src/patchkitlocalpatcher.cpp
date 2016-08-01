#include "patchkitlocalpatcher.h"
#include "launcherexception.h"

#include <QDebug>
#include <QDir>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

int PatchKitLocalPatcher::getVersion() const
{
    qDebug() << "Reading version of installed PatchKit patcher.";
    QFile file(QDir::cleanPath(patcherDirectory + "/" + versionInfoFileName));

    if(!file.open(QFile::ReadOnly))
    {
        throw LauncherException("Couldn't open version info file.");
    }

    QString fileContents(file.readAll());

    file.close();

    qDebug() << QString("Parsing string value to number - %1").arg(fileContents).toStdString().c_str();

    bool intParseResult;

    int version = fileContents.toInt(&intParseResult);

    if(!intParseResult)
    {
        throw LauncherException("Couldn't read version info file.");
    }

    qDebug() << QString("Successfully parsed string value to number - %1").arg(QString::number(version)).toStdString().c_str();

    return version;
}

bool PatchKitLocalPatcher::isInstalled() const
{
    QList<QString> filesToCheck;

    qDebug() << "Checking whether patcher is installed.";

    filesToCheck << QDir::cleanPath(patcherDirectory + "/" + installationInfoFileName);
    filesToCheck << QDir::cleanPath(patcherDirectory + "/" + versionInfoFileName);
    filesToCheck << QDir::cleanPath(patcherDirectory + "/" + manifsetFileName);

    for(int i = 0; i < filesToCheck.size(); i++)
    {
        if(!QFile::exists(filesToCheck[i]))
        {
            qDebug() << QString("%1 doesn't exists. Patcher is not installed.").arg(filesToCheck[i]).toStdString().c_str();
            return false;
        }
        else
        {
            qDebug() << QString("%1 exists.").arg(filesToCheck[i]).toStdString().c_str();
        }
    }

    qDebug() << "Patcher is installed.";

    return true;
}

void PatchKitLocalPatcher::install(const QString& t_filePath, int t_version) const
{
    //TODO:
}

void PatchKitLocalPatcher::uninstall() const
{
    qDebug() << "Reading installation info.";
    QFile installationInfoFile(QDir::cleanPath(patcherDirectory + "/" + installationInfoFileName));

    if(!installationInfoFile.exists())
    {
        qWarning() << "Installation info missing! Assuming that patcher is uninstalled.";
    }
    else
    {
        if(!installationInfoFile.open(QFile::ReadOnly))
        {
            throw LauncherException("Couldn't open installation info file.");
        }

        QString installationInfoContents(installationInfoFile.readAll());

        QStringList installFiles = installationInfoContents.split(QChar('\n'));

        for(int i = 0; i < installFiles.size(); i++)
        {
            QString fileFullPath = QDir::cleanPath(patcherDirectory + "/" + installFiles[i]);

            QFileInfo fileInfo(fileFullPath);

            if(fileInfo.exists())
            {
                if(fileInfo.isFile())
                {
                    qDebug() << QString("Deleting file %1").arg(fileFullPath).toStdString().c_str();
                    QFile::remove(fileFullPath);
                }
                else if(fileInfo.isDir())
                {
                    qDebug() << QString("Deleting direcetory %1").arg(fileFullPath).toStdString().c_str();
                    QDir(fileFullPath).removeRecursively();
                }
            }
        }

        qDebug() << "Deleting installation info.";
        installationInfoFile.close();
        installationInfoFile.remove();
    }

    qDebug() << "Deleting version info.";
    QFile versionInfoFile(QDir::cleanPath(patcherDirectory + "/" + versionInfoFileName));
    if(versionInfoFile.exists())
    {
        versionInfoFile.remove();
    }
}

void PatchKitLocalPatcher::start(const QString& t_applicationSecret) const
{
    QString exeFileName;
    QString exeArguments;

    readPatcherManifset(exeFileName, exeArguments);

    qDebug() << QString("Preparing run command from format - %1 %2").arg(exeFileName, exeArguments).toStdString().c_str();

    qDebug() << "Formating run command.";

    exeFileName = formatPatcherManifest(exeFileName, t_applicationSecret);
    exeFileName = formatPatcherManifest(exeArguments, t_applicationSecret);

    qDebug() << QString("Run command - %1 %2").arg(exeFileName, exeArguments).toStdString().c_str();


}

void PatchKitLocalPatcher::readPatcherManifset(QString& t_exeFileName, QString& t_exeArguments) const
{
    qDebug() << "Reading patcher manifest.";

    QFile manifestFile(QDir::cleanPath(patcherDirectory + "/" + manifsetFileName));

    if(!manifestFile.open(QFile::ReadOnly))
    {
        throw LauncherException("Couldn't open patcher manifest file.");
    }

    QString manifestFileContents(manifestFile.readAll());

    QJsonDocument jsonDocument = QJsonDocument::fromJson(manifestFileContents.toUtf8());

    if(!jsonDocument.isObject())
    {
        throw LauncherException("Invaild format of patcher manifest file.");
    }

    QJsonObject jsonObject = jsonDocument.object();

    if(!jsonObject.contains("exe_fileName") || !jsonObject.contains("exe_arguments"))
    {
        throw LauncherException("Invaild format of patcher manifest file.");
    }

    QJsonValue exeFileNameJsonValue = jsonObject.value("exe_fileName");
    QJsonValue exeArgumentsJsonValue = jsonObject.value("exe_arguments");

    if(!exeFileNameJsonValue.isString() || exeArgumentsJsonValue.isString())
    {
        throw LauncherException("Invaild format of patcher manifest file.");
    }

    t_exeFileName = exeFileNameJsonValue.toString();
    t_exeArguments = exeArgumentsJsonValue.toString();
}

QString PatchKitLocalPatcher::formatPatcherManifest(const QString& t_stringToFormat, const QString& t_applicationSecret) const
{
    QString result(t_stringToFormat);

    result = result.replace("{installdir}", installationDirectory);
    result = result.replace("{exedir}", patcherDirectory);
    result = result.replace("{secret}", t_applicationSecret);

    return result;
}
