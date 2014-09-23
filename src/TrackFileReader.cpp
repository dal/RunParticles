
#include "TrackFileReader.h"

#include <stdio.h>

#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include <QtDebug>

#include "FitFileReader.h"
#include "XmlHandler.h"
#include "GpxHandler.h"
#include "TcxHandler.h"

TrackFileReader::TrackFileReader()
{
    
}

bool
TrackFileReader::read(const QString &path, 
                      QList<Track*> *tracks,
                      std::string *whyNot) const
{
    bool success = false;
    QFile theFile(path);
    if (!theFile.exists()) {
        if (whyNot) {
            QString tmpWhyNot = QString("'%0' doesn't exist").arg(path);
            *whyNot = tmpWhyNot.toStdString();
        }
        success = false;
    } else if (FitFileReader::IsFitFile(path)) {
        success = _readFit(path, tracks, whyNot);
    } else {
        success = _readXml(theFile, tracks, whyNot);
    }
    return success;
}

bool
TrackFileReader::_readXml(QFile &theFile,
                          QList<Track*> *tracks,
                          std::string *whyNot) const
{
    XmlHandler xmlHandler;
    QXmlSimpleReader reader;
    reader.setContentHandler(&xmlHandler);
    reader.setErrorHandler(&xmlHandler);
    
    QXmlInputSource source(&theFile);
    
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
        *whyNot = "Unrecognized file type";
        return false;
        break;
        }
    }
    source.reset();
    reader.parse(&source, true /*incremental*/);
    while (reader.parseContinue()) { };
    return true;
}

bool
TrackFileReader::_readFit(const QString &path,
                          QList<Track*> *tracks,
                          string *whyNot) const
{
    FitFileReader reader(tracks);
    return reader.readFile(path, whyNot);
}

