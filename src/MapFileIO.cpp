//
//  MapFileWriter.cpp
//  RunParticles
//
//  Created by Doug Letterman on 8/19/14.
//
//

#include "MapFileIO.h"
#include "MapFileHandler.h"

#include <QDomDocument>
#include <QDomElement>
#include <QIODevice>
#include <QSaveFile>
#include <QTextStream>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

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
    _dirty(false)
{
    
}

bool
MapFileIO::writeMapFile()
{
    QDomDocument doc("RunParticlesMap");
    QDomElement root = doc.createElement("RunParticlesMap");
    doc.appendChild(root);
    QDomElement trackFileList = doc.createElement("trackFileList");
    root.appendChild(trackFileList);
    QString trackFilePath;
    foreach(trackFilePath, _trackFiles) {
        QDomElement trackFileElem = doc.createElement("trackFile");
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
    _dirty = false;
    return true;
}

bool
MapFileIO::loadMapFile(char **whyNot)
{
    QFile *theFile = new QFile(_filename);
    if (!theFile->exists()) {
        if (whyNot) {
            QString tmpWhyNot = QString("'%0' doesn't exist").arg(_filename);
            *whyNot = tmpWhyNot.toLocal8Bit().data();
        }
        return false;
    }
    QXmlSimpleReader reader;
    MapFileHandler handler;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    
    QXmlInputSource source(theFile);
    
    reader.parse(&source, true /*incremental*/);
    while (reader.parseContinue()) { };
    _trackFiles = handler.getTrackFiles();
    _viewArea = handler.getViewArea();
    return true;
}

void
MapFileIO::addTrackFile(const QString &filename)
{
    _trackFiles.append(filename);
    _dirty = true;
}

void
MapFileIO::setFilename(const QString &filename)
{
    _filename = filename;
}

void
MapFileIO::clear()
{
    _filename.clear();
    _trackFiles.clear();
    _dirty = false;
}

void
MapFileIO::setViewArea(const LonLatBox &bbox)
{
    _viewArea = bbox;
}

