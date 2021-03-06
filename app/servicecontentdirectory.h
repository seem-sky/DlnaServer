#ifndef SERVICECONTENTDIRECTORY_H
#define SERVICECONTENTDIRECTORY_H

#include <QObject>
#include "cached/dlnacachedrootfolder.h"
#include "Http/httprequest.h"
#include "soapaction.h"
#include "soapactionresponse.h"
#include "didllite.h"

class ServiceContentDirectory : public QObject
{
    Q_OBJECT

public:
    explicit ServiceContentDirectory(QString host, int port, QObject *parent = 0);

    void setNetworkAccessManager(QNetworkAccessManager *nam);

    void reply(HttpRequest *request);

private:
    DlnaResource *getDlnaResource(const QString &hostaddress, const QString &objId);

signals:
    void addFolderSignal(QString folder);
    void folderAdded(QString folder);
    void error_addFolder(QString folder);
    void scanFolder(QString path);

    void addNetworkLinkSignal(const QString url);
    void linkAdded(QString url);
    void error_addNetworkLink(QString url);
    void brokenLink(QString url, QString title);

    void reloadLibrarySignal(const QStringList localFolder);

    void updateMediaData(const QString &filename, const QHash<QString, QVariant> &data);
    void updateMediaFromId(const int &id, const QHash<QString, QVariant> &data);
    void incrementCounterPlayedSignal(const QString &filename);

    void servingRendererSignal(QString ip, const QString &mediaName);

    // emit signal when serving is finished
    //   status = 0 if serving finished successfully
    //   status = 1 if error occurs
    void servingFinishedSignal(QString host, QString filename, int status);


private slots:
    void _addFolder(const QString &folder);

    void folderAddedSlot(QString folder);

    void reloadLibrary();

    void readTimeSeekRange(const QString &data, qint64 *start, qint64*end);

    void streamReadyToOpen();

    void servingMedia(QString filename, int playedDurationInMs);

    void servingFinished(QString host, QString filename, int status);

    void dlnaContentUpdated();

    void mediaRendererDestroyed(const QString &hostaddress);


private:
    DlnaCachedRootFolder rootFolder;

    // root folder containing DLNA nodes
    QStringList listFolderAdded;

    QHash<QString, DlnaResource*> m_dlnaresources;
};

#endif // SERVICECONTENTDIRECTORY_H
