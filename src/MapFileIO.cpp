//
//  MapFileWriter.cpp
//  RunParticles
//
//  Created by Doug Letterman on 8/19/14.
//
//

#include "MapFileIO.h"
#include "MapFileHandler.h"
#include "TrackLayer.h"

#include <stdio.h>

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFileInfo>
#include <QIODevice>
#include <QSaveFile>
#include <QTextStream>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

#define WHY_NOT(format, ...) \
    if (whyNot) { \
        sprintf(*whyNot , format, ##__VA_ARGS__); \
}

const QByteArray MapFileIO::_fileHeaderByteArray("RunParticlesMap");

const quint16 MapFileIO::_fileVersion = 1;

QDomElement _encodePoint(QDomDocument &doc, const QString &name,
                         const LonLat &pt)
{
    QDomElement myEl = doc.createElement(name);
    myEl.setAttribute("lattitudeDegrees", pt.lat());
    myEl.setAttribute("longitudeDegrees", pt.lon());
    return myEl;
};

LonLat _getPoint()
{
    LonLat myPt;
    
    return myPt;
}

MapFileIO::MapFileIO(QObject *parent) :
    QObject(parent),
    _isLegacyMapFile(false)
{
    
}

bool
MapFileIO::writeLegacyMapFile(bool relativePaths)
{
    // Deprecated: Use exportMap now
    QDomDocument doc("RunParticlesMap");
    QDomElement root = doc.createElement("RunParticlesMap");
    doc.appendChild(root);
    QDomElement trackFileList = doc.createElement("trackFileList");
    root.appendChild(trackFileList);
    QString trackFilePath;
    foreach(trackFilePath, _trackFiles) {
        QDomElement trackFileElem = doc.createElement("trackFile");
        if (relativePaths)
            trackFilePath = relativeizePath(trackFilePath);
        trackFileElem.setAttribute("path", trackFilePath);
        trackFileList.appendChild(trackFileElem);
    }
    QDomElement viewAreaEl = doc.createElement("viewArea");
    viewAreaEl.appendChild(_encodePoint(doc, "upperLeft", _viewArea.upperLeft));
    viewAreaEl.appendChild(_encodePoint(doc, "lowerRight",
                                        _viewArea.lowerRight));
    root.appendChild(viewAreaEl);
    QSaveFile saveFile(_filename);
    saveFile.open(QIODevice::WriteOnly);
    QTextStream ts(&saveFile);
    ts << doc.toString();
    saveFile.commit();
    return true;
}


bool
MapFileIO::exportMap(const LayerPtrList& layers)
{
    QSaveFile saveFile(_filename);
    saveFile.open(QIODevice::WriteOnly);
    QDataStream dataStream(&saveFile);
    dataStream.setVersion( QDataStream::Qt_4_6 );
    dataStream.writeRawData(_fileHeaderByteArray.constData(),
                            _fileHeaderByteArray.size() );
    dataStream << _fileVersion;
    dataStream << _viewArea.upperLeft;
    dataStream << _viewArea.lowerRight;
    QList<Track> tracks;
    LayerPtrList::const_iterator it;
    for (it = layers.begin(); it != layers.end(); it++)
    {
        std::shared_ptr<TrackLayer> myLayer =
        std::dynamic_pointer_cast<TrackLayer>(*it);
        if (myLayer) {
            Track theTrack = *myLayer->getTrack();
            tracks.append(theTrack);
        }
    }
    dataStream << tracks;
    saveFile.commit();
    return true;
}

bool
MapFileIO::loadLegacyMapFile(char **whyNot)
{
    QFile *theFile = new QFile(_filename);
    if (!theFile->exists()) {
        WHY_NOT("File '%s' doesn't exist", _filename.toLocal8Bit().data());
        return false;
    }
    QXmlSimpleReader reader;
    MapFileHandler handler;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    
    QXmlInputSource source(theFile);
    
    reader.parse(&source, true /*incremental*/);
    while (reader.parseContinue()) { };
    QString trackFilePath;
    foreach(trackFilePath, handler.getTrackFiles()) {
        _trackFiles.append(absoluteizePath(trackFilePath));
    }
    _viewArea = handler.getViewArea();
    return true;
}

bool
MapFileIO::loadMapFile(QList<Track*> &tracks, char **whyNot)
{
    QFile *theFile = new QFile(_filename);
    if (!theFile->exists()) {
        WHY_NOT("File '%s' does not exist", _filename.toLocal8Bit().data());
        return false;
    }
    theFile->open( QIODevice::ReadOnly );
    QDataStream dataStream(theFile);
    dataStream.setVersion( QDataStream::Qt_4_6 );
    const int len  = _fileHeaderByteArray.size();
    QByteArray actualFileHeaderByteArray( len, '\0' );
    dataStream.readRawData( actualFileHeaderByteArray.data(), len );
    
    if ( actualFileHeaderByteArray != _fileHeaderByteArray )
    {
        // prefixes don't match
        theFile->close();
        if ( actualFileHeaderByteArray.startsWith("<!DOCTYPE RunParticlesMap")) {
            // It looks like the legacy-style list of paths in XML
            _isLegacyMapFile = true;
            return loadLegacyMapFile(whyNot);
        }
        WHY_NOT("Map file prefix mismatch error.");
        return false;
    }
    quint16 actualFileVersion = 0;
    dataStream >> actualFileVersion;
    if ( actualFileVersion == 0 || actualFileVersion > _fileVersion )
    {
        // file is from a future version that we don't know how to load
        WHY_NOT("Compatibility error: actualFileVersion = "
                "%d and fileVersion = %d", actualFileVersion, _fileVersion );
        theFile->close();
        return false;
    }
    dataStream >> _viewArea.upperLeft;
    dataStream >> _viewArea.lowerRight;
    QList <Track> trackData;
    dataStream >> trackData;
    Track thisTrack;
    foreach(thisTrack, trackData) {
        Track* trackPtr = new Track(thisTrack);
        tracks.append(trackPtr);
    }
    theFile->close();
    return true;
}

void
MapFileIO::setFilename(const QString &filename)
{
    _filename = filename;
    _isLegacyMapFile = false; // (we don't know yet)
}

void
MapFileIO::clear()
{
    _filename.clear();
    _trackFiles.clear();
}

void
MapFileIO::setViewArea(const LonLatBox &bbox)
{
    _viewArea = bbox;
}

QString
MapFileIO::relativeizePath(const QString &path)
{
    QFileInfo mapInfo(_filename);
    QDir mapDir = mapInfo.absoluteDir();
    return mapDir.relativeFilePath(path);
}

QString
MapFileIO::absoluteizePath(const QString &path)
{
    QFileInfo pathInfo(path);
    if (pathInfo.isAbsolute())
        return path;
    QFileInfo mapInfo(_filename);
    QDir mapDir = mapInfo.absoluteDir();
    return mapDir.absoluteFilePath(path);
}

