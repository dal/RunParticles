//
//  MapFileHandler.cpp
//  RunParticles
//
//  Created by Doug Letterman on 8/25/14.
//
//

#include "MapFileHandler.h"

#include <QDebug>

#include "Types.h"

LonLat _decodePoint(const QXmlAttributes &atts)
{
    LonLat pt;
    bool ok = false;
    int latIdx = atts.index(QString(""), "lattitudeDegrees");
    if (latIdx != -1) {
        QString lat = atts.value(latIdx);
        pt.y = lat.toDouble(&ok);
        if (!ok) {
            qWarning() << "Invalid LonLat element latitudeDegrees: '"
            << lat << "'";
        }
    }
    int lonIdx = atts.index(QString(""), "longitudeDegrees");
    if (lonIdx != -1) {
        QString lon = atts.value(lonIdx);
        pt.x = lon.toDouble(&ok);
        if (!ok) {
            qWarning() << "Invalid LonLat element longitudeDegrees: '"
            << lon << "'";
        }
    }
    return pt;
};

MapFileHandler::MapFileHandler() :
    _inViewArea(false)
{
    
}

bool
MapFileHandler::startDocument()
{
    _trackFiles.clear();
    return true;
}

bool
MapFileHandler::startElement(const QString & namespaceURI,
                             const QString & localName,
                             const QString & qName,
                             const QXmlAttributes & atts )
{
    if (localName == "trackFile") {
        _inViewArea = false;
        int pathIdx = atts.index(QString(""), "path");
        if (pathIdx != -1) {
            _trackFiles.append(atts.value(pathIdx));
        }
    } else if (localName == "viewArea") {
        _inViewArea = true;
    } else if (localName == "upperLeft" && _inViewArea) {
        _viewArea += _decodePoint(atts);
    } else if (localName == "lowerRight" && _inViewArea) {
        _viewArea += _decodePoint(atts);
    }
    return true;
}

bool
MapFileHandler::endElement(const QString & /* namespaceURI */,
                       const QString & localName,
                       const QString & /* qName */)
{
    if (localName == "viewArea")
        _inViewArea = false;
    return true;
}
