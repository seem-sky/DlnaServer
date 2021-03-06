#include "applicationworker.h"

ApplicationWorker::ApplicationWorker(QNetworkAccessManager *nam, QObject *parent):
    Worker(parent),
    m_nam(nam)
{

}

void ApplicationWorker::scanFolder(QString path)
{
//    emit processStarted();

    qDebug() << this << "scan folder" << path;

    QSqlDatabase db = GET_DATABASE("MEDIA_DATABASE");
    if (db.isValid() && db.isOpen())
    {
        DlnaCachedRootFolder root("HOST", 80);
        root.readDirectory(QDir(path));

//        emit processOver();
    }
    else
    {
        qCritical() << "database is not valid, unable to scan folder" << path;
//        emit errorDuringProcess(QString("database is not valid, unable to scan folder %1").arg(path.absolutePath()));
    }
}

void ApplicationWorker::checkNetworkLink()
{
    emit processStarted();

    qDebug() << "start check network links.";
    if (m_nam)
    {
        // initialize database in current Thread
        QSqlDatabase database = GET_DATABASE("MEDIA_DATABASE");

        if (database.isValid() && database.isOpen())
        {
            emit progress(0);

            MediaLibrary library;

            int nb = 0;

            QSqlQuery query = library.getAllNetworkLinks();

            query.last();
            int total = query.at();
            qDebug() << total << "links to check.";

            query.exec();
            while (query.next())
            {
                emit progress(nb*100/total);
                if (processWasAborted())
                    break;  // Abort process

                QString name = QString("%1 %2").arg(query.value("artist").toString()).arg(query.value("title").toString());

                QString url(query.value("filename").toString());
                bool isReachable = query.value("is_reachable").toBool();

                QScopedPointer<DlnaYouTubeVideo, QScopedPointerDeleteLater> movie(new DlnaYouTubeVideo("HOST", 80));
                movie->moveToThread(m_nam->thread());
                QObject::connect(m_nam->thread(), SIGNAL(finished()), movie.data(), SLOT(deleteLater()));
                movie->setNetworkAccessManager(m_nam);

                movie->setUrl(url);
                bool res = movie->waitUrl(30000);

                if (!(res && movie->isValid()))
                {
                    if (isReachable)
                    {
                        if (res && !movie->unavailableMessage().isEmpty())
                        {
                            emit addMessage(name, QString("put OFFLINE - %1").arg(movie->unavailableMessage()));

                            QHash<QString, QVariant> data;
                            data["is_reachable"] = QVariant(0);

                            library.updateFromFilename(url, data);
                        }
                    }
                    else
                    {
                        emit addMessage(name, QString("link is still unreachable - %1").arg(movie->unavailableMessage()));
                    }
                }
                else
                {
                    if (!isReachable)
                    {
                        emit addMessage(name, "put ONLINE");

                        QHash<QString, QVariant> data;
                        data["is_reachable"] = QVariant(1);

                        library.updateFromFilename(url, data);

                        // refresh data
                        emit refresh(QUrl(url).toString());
                    }
                }

                ++nb;
            }

            emit processOver(QString("%1 network links checked.").arg(total));
        }
        else
        {
            qCritical() << "database is not valid, unable to check network links.";
            emit errorDuringProcess( "database is not valid, unable to check network links.");
        }
    }
    else
    {
        qCritical() << "QNetworkAccessManager is not initialized, unable to check network links.";
        emit errorDuringProcess("QNetworkAccessManager is not initialized, unable to check network links.");
    }
}

void ApplicationWorker::scanVolumeInfo()
{
    emit processStarted();
    qDebug() << "start volume information scan";

    if (m_nam)
    {
        // initialize database in current Thread
        QSqlDatabase database = GET_DATABASE("MEDIA_DATABASE");

        if (database.isValid() && database.isOpen())
        {
            MediaLibrary library;

            if (library.isValid())
            {
                QSqlQuery query(database);
                query.prepare("SELECT media.id, media.filename, mime_type.name from media LEFT OUTER JOIN mime_type ON media.mime_type=mime_type.id WHERE media.is_reachable=1");

                if (query.exec())
                {
                    while (query.next())
                    {
                        if (processWasAborted())
                            break;  // Abort process

                        int idMedia = query.record().value("id").toInt();
                        QString filename = query.record().value("filename").toString();
                        QString mime_type = query.record().value("name").toString();
                        QHash<QString, double> volumeInfo = library.volumeInfo(idMedia);

                        if (volumeInfo.isEmpty())
                        {
                            if (mime_type.startsWith("audio/"))
                            {
                                qDebug() << "Analyze audio" << filename;

                                DlnaMusicTrackFile track(filename, "HOST", 80);
                                if (!library.setVolumeInfo(idMedia, track.volumeInfo()))
                                    qCritical() << "Unable to set volume information for" << filename;
                            }
                            else if (mime_type.startsWith("video/")  && !filename.startsWith("http"))
                            {
                                qDebug() << "Analyze local video" << filename;

                                DlnaVideoFile movie(filename, "HOST", 80);
                                if (!library.setVolumeInfo(idMedia, movie.volumeInfo(-1)))
                                    qCritical() << "Unable to set volume information for" << filename;
                            }
                            else if (mime_type.startsWith("video/") && filename.startsWith("http"))
                            {
                                qDebug() << "Analyze internet video" << filename;

                                QScopedPointer<DlnaYouTubeVideo, QScopedPointerDeleteLater> video(new DlnaYouTubeVideo("HOST", 80));
                                video->moveToThread(m_nam->thread());
                                QObject::connect(m_nam->thread(), SIGNAL(finished()), video.data(), SLOT(deleteLater()));
                                video->setNetworkAccessManager(m_nam);
                                video->setUrl(filename);
                                bool res = video->waitUrl(30000);

                                if (res && video->isValid())
                                {
                                    if (!library.setVolumeInfo(idMedia, video->volumeInfo(-1)))
                                        qCritical() << "Unable to set volume information for" << filename;
                                }
                                else
                                {
                                    qCritical() << "NO VOLUME INFO (TIMEOUT)" << filename;
                                }
                            }
                            else
                            {
                                qWarning() << "NO VOLUME INFO" << filename;
                            }
                        }
                    }
                }
                else
                {
                    qCritical() << "ERROR in request" << query.lastError().text();
                }
            }

            emit processOver();
        }
        else
        {
            qCritical() << "database is not valid, unable to scan volume.";
            emit errorDuringProcess( "database is not valid, unable to scan volume.");
        }
    }
    else
    {
        qCritical() << "QNetworkAccessManager is not initialized, unable to scan volume.";
        emit errorDuringProcess("QNetworkAccessManager is not initialized, unable to scan volume.");
    }
}
