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

class MapFileHandler : public QXmlDefaultHandler
{
    
public:
    
    MapFileHandler();
    
    bool startDocument();
    bool startElement(const QString&,
                      const QString&,
                      const QString&,
                      const QXmlAttributes&);
    QStringList getTrackFiles() const { return _trackFiles; }
    
protected:
    
    QStringList _trackFiles;
    
};


#endif /* defined(__RunParticles__MapFileHandler__) */
