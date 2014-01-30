#ifndef DLNAITEM_H
#define DLNAITEM_H

#include <QFileInfo>
#include <QFile>

#include "logger.h"
#include "dlnaresource.h"
#include "metadata.h"

class DlnaItem : public DlnaResource
{
    Q_OBJECT
public:
    explicit DlnaItem(Logger* log, QString filename, QString host, int port, QObject *parent = 0);

    virtual bool discoverChildren() { return true; }

    // Any resource needs to represent the container or item with a String.
    // String to be showed in the UPNP client.
    virtual QString getName() const;

    virtual QString getSystemName() const;

    // Returns the DisplayName that is shown to the Renderer.
    virtual QString getDisplayName();

    virtual bool isFolder() const { return false; }

    QFileInfo getFileInfo() const { return fileinfo; }

    // Returns an InputStream of this DLNA node.
    QIODevice* getStream();

    // Returns the process for transcoding
    virtual TranscodeProcess* getTranscodeProcess(HttpRange* range, long timeseek_start=-1, long timeseek_end=-1) = 0;

    // return true if the track shall be transcoded
    virtual bool toTranscode() const = 0;

    void setTranscodeFormat(TranscodeFormatAvailable format);

    // Returns the mimeType for this DLNA node.
    virtual QString mimeType() = 0;

    // return the size of the audio track
    long size();

    // return the length in seconds of the media
    int getLengthInSeconds();
    int getLengthInMilliSeconds();

    // returns the bitrate of the audio track
    virtual int bitrate() = 0;

    QString getProtocolInfo();

    QString getDlnaContentFeatures() const;

    QString getdlnaOrgOpFlags() const { return dlnaOrgOpFlags; }
    void setdlnaOrgOpFlags (QString arg) { dlnaOrgOpFlags=arg; }

    QString getdlnaOrgPN() const { return dlnaOrgPN; }
    void setdlnaOrgPN(QString arg) { dlnaOrgPN=arg; }

signals:

public slots:

protected:
    QFileInfo fileinfo;

    MetaData mediaTag;

    QMimeType mime_type;
    QString host;
    int port;

    TranscodeFormatAvailable transcodeFormat;

    /*
     * DLNA.ORG_OP flags
     *
     * Two booleans (binary digits) which determine what transport operations the renderer is allowed to
     * perform (in the form of HTTP request headers): the first digit allows the renderer to send
     * TimeSeekRange.DLNA.ORG (seek by time) headers; the second allows it to send RANGE (seek by byte)
     * headers.
     *
     *    00 - no seeking (or even pausing) allowed
     *    01 - seek by byte
     *    10 - seek by time
     *    11 - seek by both
     *
     * See here for an example of how these options can be mapped to keys on the renderer's controller:
     * http://www.ps3mediaserver.org/forum/viewtopic.php?f=2&t=2908&p=12550#p12550
     *
     * Note that seek-by-byte is the preferred option for streamed files [1] and seek-by-time is the
     * preferred option for transcoded files.
     *
     * [1] see http://www.ps3mediaserver.org/forum/viewtopic.php?f=6&t=15841&p=76201#p76201
     *
     * seek-by-time requires a) support by the renderer (via the SeekByTime renderer conf option)
     * and b) support by the transcode engine.
     *
     * The seek-by-byte fallback doesn't work well with transcoded files [2], but it's better than
     * disabling seeking (and pausing) altogether.
     *
     * [2] http://www.ps3mediaserver.org/forum/viewtopic.php?f=6&t=3507&p=16567#p16567 (bottom post)
     */
    QString dlnaOrgOpFlags;

    // DLNA.ORG_PN
    QString dlnaOrgPN;

    virtual void updateDLNAOrgPn() = 0;
};

#endif // DLNAITEM_H