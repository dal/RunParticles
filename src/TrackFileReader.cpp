
#include "TrackFileReader.h"

#include <stdio.h>

#include <QXmlInputSource>
#include <QXmlSimpleReader>

#include "XmlHandler.h"
#include "GpxHandler.h"
#include "TcxHandler.h"

TrackFileReader::TrackFileReader()
{
    
}

bool
TrackFileReader::read(const QString &path, 
                      QList<Track*> *tracks, 
                      char **whyNot) const
{
    QFile *theFile = new QFile(path);
    if (!theFile->exists()) {
        if (whyNot) {
            sprintf(*whyNot, "'%s' doesn't exist", path.toAscii().constData());
        }
        return false;
    }
    XmlHandler xmlHandler;
    QXmlSimpleReader reader;
    reader.setContentHandler(&xmlHandler);
    reader.setErrorHandler(&xmlHandler);
    
    QXmlInputSource source(theFile);
    
    reader.parse(&source, true /*incremental*/);
    while (reader.parseContinue()) { };
    TrackFileType fileType = xmlHandler.getType();
    switch (fileType) {
    case TrackFileType_Gpx:
        {
        GpxHandler gpxHandler(tracks);
        reader.setContentHandler(&gpxHandler);
        reader.setErrorHandler(&gpxHandler);
        break;
        }
    case TrackFileType_Tcx:
        {
        TcxHandler tcxHandler(tracks);
        reader.setContentHandler(&tcxHandler);
        reader.setErrorHandler(&tcxHandler);
        break;
        }
    default:
        {
        sprintf(*whyNot, "Unrecognized file type");
        return false;
        break;
        }
    }
    source.reset();
    reader.parse(&source, true /*incremental*/);
    while (reader.parseContinue()) { };
    return true;
}

