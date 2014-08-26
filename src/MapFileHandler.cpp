//
//  MapFileHandler.cpp
//  RunParticles
//
//  Created by Doug Letterman on 8/25/14.
//
//

#include "MapFileHandler.h"

MapFileHandler::MapFileHandler()
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
        
        int pathIdx = atts.index(QString(""), "path");
        if (pathIdx != -1) {
            _trackFiles.append(atts.value(pathIdx));
        }
    }
    return true;
}
