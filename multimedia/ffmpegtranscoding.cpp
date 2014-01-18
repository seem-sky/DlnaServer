#include "ffmpegtranscoding.h"

FfmpegTranscoding::FfmpegTranscoding(QObject *parent) :
    TranscodeProcess(parent)
{
}

bool FfmpegTranscoding::initialize(HttpRange *range, QString filePath, int lengthInSeconds, TranscodeFormatAvailable transcodeFormat, int bitrate, QStringList audioLanguages, QStringList subtitleLanguages, QString framerate) {

    QString program = "/Users/doudou/workspace/DLNA_server/exe/mencoder";
    QString fontFile = "/Users/doudou/workspace/DLNA_server/exe/LucidaSansRegular.ttf";

    QStringList arguments;
    if (range != 0 && !range->isNull()) {
        if (range->getStartByte() > 0) {
            double start_position = double(range->getStartByte())/double(range->getSize())*double(lengthInSeconds);
            arguments << "-ss" << QString("%1").arg(long(start_position));
        }
    }

    arguments << filePath;

    if (transcodeFormat == MPEG2_AC3) {

        // set container format to MPEG
        arguments << "-of" << "mpeg";
        arguments << "-mpegopts" << "format=mpeg2:muxrate=500000:vbuf_size=1194:abuf_size=64";

        // set audio options
        arguments << "-oac" << "lavc";
        arguments << "-channels" << "6";
        arguments << "-af" << "lavcresample=48000";
        arguments << "-srate" << "48000";

        // set video options
        arguments << "-ovc" <<  "lavc";
        arguments << "-lavdopts" <<  "fast:debug=0";
        arguments << "-lavcopts" << "autoaspect=1:vcodec=mpeg2video:acodec=ac3:abitrate=448:keyint=25:vrc_maxrate=9800:vrc_buf_size=1835:vbitrate=5000";

        // set font file
        arguments << "-font" << fontFile;

        // set subtitles options
        arguments << "-ass" << "-ass-color" << "ffffff00" << "-ass-border-color" << "00000000" << "-ass-font-scale" << "1.4";
        arguments << "-ass-force-style" << QString("FontName=%1,Outline=1,Shadow=1,MarginV=10").arg(fontFile);

        // choose audio and subtitle language
        if (audioLanguages.contains("French")) {
            arguments << "-aid" << QString("%1").arg(audioLanguages.indexOf("French"));
        } else {
            if (subtitleLanguages.contains("French")) {
                arguments << "-noautosub" << "-sid" << QString("%1").arg(subtitleLanguages.indexOf("French"));
            } else if (subtitleLanguages.contains("English")) {
                arguments << "-noautosub" << "-sid" << QString("%1").arg(subtitleLanguages.indexOf("English"));
            }
        }

        // set frame rate
        if (!framerate.isEmpty()) {
            if (framerate == "23.976") {
                arguments << "-ofps" << "24000/1001";
            } else if (framerate == "29.97") {
                arguments << "-ofps" << "30000/1001";
            } else {
                arguments << "-ofps" << framerate;
            }
        }

    } else {
        // invalid transcode format
        return false;
    }

    if (range != 0 && !range->isNull()) {
        if (range->getLength() > 0) {
            if (range->getHighRange() >= 0) {
                // calculate the endpos in seconds
                double endpos = double(range->getLength())/double(range->getSize())*double(lengthInSeconds);
                arguments << "-endpos" << QString("%1").arg(long(endpos));
            }
        } else {
            // invalid length
            arguments << "-endpos 0";
        }

    }

    // set output = pipe
    arguments << "-o" << "-";

    // set option on loglevel
//        arguments << "-really-quiet";
//        arguments << "-msglevel" << "statusline=2";

    setProgram(program);
    setArguments(arguments);

    return true;
}
