#ifndef GPXHANDLER_H
#define GPXHANDLER_H

#include <QFile>
#include <QString>
#include <QList>
#include <vector>

#include "Types.h"

using namespace std;

class GpxHandler
{

public:

    GpxHandler(QList<Track*>*);
    
    /* stream reader parser */
    bool parse(QFile *gpxFile);
    
    QString getError() const { return _error; };
    
protected:
    
    QList<Track*> *_tracks;
    
    TrackPoint _currentPoint;
    
    Track *_currentTrack;
    
    QString _error;
    
    int _depth;
    
    bool _inPoint, _foundLat, _foundLon, _foundTime;

};

#endif
