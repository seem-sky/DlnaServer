#ifndef DLNACACHEDNETWORKVIDEO_H
#define DLNACACHEDNETWORKVIDEO_H

#include "dlnacachedvideo.h"
#include "ffmpegtranscoding.h"

class DlnaCachedNetworkVideo : public DlnaCachedVideo
{
    Q_OBJECT

public:
    explicit DlnaCachedNetworkVideo(Logger* log, MediaLibrary* library, int idMedia, QString host, int port, QObject *parent = 0);

    //returns the size of the source
    virtual qint64 sourceSize() const { return -1; }

    virtual QString metaDataTitle() const { if (library != 0) return library->getmetaData("title", idMedia).toString(); else return QString(); }

    // Returns the time where the media has been stopped during last play
    virtual qint64 getResumeTime() const { return 0; }

protected:
    // Returns the process for transcoding
    virtual TranscodeProcess* getTranscodeProcess();

};

#endif // DLNACACHEDNETWORKVIDEO_H
