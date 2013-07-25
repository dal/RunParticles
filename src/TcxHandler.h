
#ifndef HANDLER_H
#define HANDLER_H

#include <QString>
#include <QList>
#include <QXmlDefaultHandler>
#include <QXmlAttributes>
#include <vector>

#include "Track.h"

using namespace std;

class TcxHandler : public QXmlDefaultHandler
{
    
public:

    TcxHandler(QList<Track*>*);
    
    bool startDocument();
    bool characters(const QString & ch);
    bool startElement(const QString&, 
                      const QString&, 
                      const QString&, 
                      const QXmlAttributes&);
    
    bool endElement(const QString&, const QString&, const QString&);
    bool endDocument();
    
protected:
    QList<Track*> *_tracks;
    
    TrackPoint *_currentPoint;
    
    Track *_currentTrack;
    
    int _depth;
    
    bool _inTime;
    
    bool _inLatitudeDegrees;
    
    bool _inLongitudeDegrees;
    
    bool _foundLat;
    
    bool _foundLon;
    
    bool _foundTime;
    
    int _startTime;
    
};

#endif
