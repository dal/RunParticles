
#ifndef TCXHANDLER_H
#define TCXHANDLER_H

#include <QFile>
#include <QString>
#include <QList>
#include <vector>

#include "Types.h"

using namespace std;

class TcxHandler
{
    
public:

    TcxHandler(QList<Track*>*);
    
    bool parse(QFile *tcxFile);
    QString getError() const { return _error; };
    
protected:
    QList<Track*> *_tracks;
    
    TrackPoint _currentPoint;
    
    Track *_currentTrack;
    
    QString _error;
    
    int _depth;
    
    bool _inPoint, _inTime, _foundLat, _foundLon, _foundTime;
    
};

#endif
