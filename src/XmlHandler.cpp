#include "XmlHandler.h"

bool
XmlHandler::startElement(const QString&,
                         const QString &localName,
                         const QString&,
                         const QXmlAttributes&)
{
    if (localName == "TrainingCenterDatabase") {
        _trackFileType = TrackFileType_Tcx;
    } else if (localName == "gpx") {
        _trackFileType = TrackFileType_Gpx;
    } else if (localName == "RunParticlesMap") {
        _trackFileType = TrackFileType_RunParticlesMap;
    } else {
        _trackFileType = TrackFileType_Unknown;
    }
    return false;
}

