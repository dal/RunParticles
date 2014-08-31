#ifndef GPXHANDLER_H
#define GPXHANDLER_H

#include <QString>
#include <QList>
#include <QXmlDefaultHandler>
#include <QXmlAttributes>
#include <vector>

#include "Types.h"

using namespace std;

class GpxHandler : public QXmlDefaultHandler
{

public:

    GpxHandler(QList<Track*>*);
    
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
    
    TrackPoint _currentPoint;
    
    Track *_currentTrack;
    
    int _depth;
    
    bool _inPoint, _inTime, _inName, _foundLat, _foundLon, _foundTime;

};

#endif
