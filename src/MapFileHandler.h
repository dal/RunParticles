//
//  MapFileHandler.h
//  RunParticles
//
//  Created by Doug Letterman on 8/25/14.
//
//

#ifndef __RunParticles__MapFileHandler__
#define __RunParticles__MapFileHandler__

#include <QString>
#include <QStringList>
#include <QList>
#include <QXmlDefaultHandler>
#include <QXmlAttributes>

#include "BoundingBox.h"

class MapFileHandler : public QXmlDefaultHandler
{
    
public:
    
    MapFileHandler();
    
    bool startDocument();
    bool startElement(const QString&,
                      const QString&,
                      const QString&,
                      const QXmlAttributes&);
    bool endElement(const QString&,
                    const QString& localName,
                    const QString&);
    QStringList getTrackFiles() const { return _trackFiles; }
    LonLatBox getViewArea() const { return _viewArea; }
    
protected:
    bool _inViewArea;
    QStringList _trackFiles;
    LonLatBox _viewArea;
    
};


#endif /* defined(__RunParticles__MapFileHandler__) */
