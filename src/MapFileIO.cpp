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
#include <QSaveFile>
#include <QTextStream>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

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
    QSaveFile saveFile(_filename);
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

