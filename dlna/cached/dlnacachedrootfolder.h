#ifndef DLNACACHEDROOTFOLDER_H
#define DLNACACHEDROOTFOLDER_H

#include "dlnarootfolder.h"

#include <qmimedatabase.h>
#include "medialibrary.h"

#include "dlnamusictrackfile.h"
#include "dlnavideofile.h"
#include "dlnacachedfolder.h"
#include "dlnacachedgroupedfoldermetadata.h"
#include "dlnarootfolder.h"
#include "dlnayoutubevideo.h"

class DlnaCachedRootFolder : public DlnaRootFolder
{
    Q_OBJECT

public:
    explicit DlnaCachedRootFolder(QString host, int port, QObject *parent = 0);

    QSqlQuery getAllNetworkLinks() { return library.getAllNetworkLinks(); }

    void setNetworkAccessManager(QNetworkAccessManager *nam);

    void readDirectory(QDir folder);

private:

    void addResource(QFileInfo fileinfo);
    void addResource(QUrl url);

signals:
    void linkAdded(QString url);
    void error_addNetworkLink(QString url);
    void scanFolder(QString path);

private slots:
    void updateLibrary(const QString &filename, const QHash<QString, QVariant> &data);
    void updateLibraryFromId(const int &id, const QHash<QString, QVariant> &data);
    void incrementCounterPlayed(const QString &filename);

    // returns true if the folder is added to Root.
    virtual bool addFolderSlot(QString path);

    void addNetworkLink(const QString &url);

    void networkLinkAnalyzed(const QString &streamingUrl);
    void networkLinkError(const QString &message);

public slots:
    void reloadLibrary(const QStringList &localFolder);

private:
    MediaLibrary library;
    QMimeDatabase mimeDb;
    DlnaRootFolder rootFolder;
    DlnaCachedFolder *recentlyPlayedChild;
    DlnaCachedFolder *resumeChild;
    DlnaCachedFolder *favoritesChild;
    DlnaCachedFolder *lastAddedChild;
    DlnaCachedGroupedFolderMetaData *youtube;
    QNetworkAccessManager *m_nam;
};

#endif // DLNACACHEDROOTFOLDER_H
