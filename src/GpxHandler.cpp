#include "GpxHandler.h"
#include <time.h>

#include "Util.h"

GpxHandler::GpxHandler(QList<Track*> *tracks) :
QXmlDefaultHandler(),
_tracks(tracks),
_currentTrack(NULL),
_depth(0),
_inTime(false),
_inName(false),
_foundLat(false),
_foundLon(false),
_foundTime(false)
{
    
}

bool
GpxHandler::startDocument()
{
    _depth = 0;
    _inTime = false;
    _inName = false;
    _foundLat = false;
    _foundLon = false;
    return true;
}

bool 
GpxHandler::startElement(const QString & namespaceURI,
                      const QString & localName, 
                      const QString & qName, 
                      const QXmlAttributes & atts )
{
    qDebug("Start: %s", localName.toAscii().constData());
    if (localName == "trk") {
        if (_depth != 0) {
            qWarning("Nested tracks!");
        }
        _currentTrack = new Track();
        _tracks->append(_currentTrack);
    } else if (localName == "name") {
        _inName = true;
    } else if (localName == "trkseg") {
        _depth += 1;
    } else if (localName == "trkpt") {
        _depth += 1;
        _foundLat = false;
        _foundLon = false;
        _currentPoint = new TrackPoint();
        bool ok = false;
        int latIdx = atts.index(QString(""), "lat");
        if (latIdx != -1) {
            QString lat = atts.value(latIdx);
            _currentPoint->pos.y = lat.toDouble(&ok);
            if (ok) {
                _foundLat = true;
            } else {
                qWarning("Invalid trk element latitude: '%s'", 
                         lat.toAscii().constData());
            }
        }
        int lonIdx = atts.index(QString(""), "lon");
        if (lonIdx != -1) {
            QString lon = atts.value(lonIdx);
            _currentPoint->pos.x = lon.toDouble(&ok);
            if (ok) {
                _foundLon = true;
            } else {
                qWarning("Invalid trk element longitude: '%s'", 
                         lon.toAscii().constData());
            }
        } else {
            qWarning("trk element was missing a valid longitude");
        }
    } else if (localName == "time") {
        if (_currentPoint) {
            _inTime = true;
        }
    }
    return true;
}

bool 
GpxHandler::characters ( const QString & ch )
{
    if (_inTime && _currentPoint) {
        _currentPoint->time = Util::parseTime(ch.toAscii().constData());
        _foundTime = true;
    } else if (_inName && _currentTrack) {
        _currentTrack->name = QString(ch);
    }
    return true;
}

bool 
GpxHandler::endElement(const QString &,
                    const QString & localName, 
                    const QString &)
{
    if (localName == "trk") {
        if (_currentTrack != NULL) {
            _currentTrack = NULL;
        }
        _depth -= 1;
    } else if (localName == "name") {
        _inName = false;
    } else if (localName == "trkseg") {
        _depth -= 1;
    } else if (localName == "trkpt") {
        _depth -= 1;
        if (_currentPoint && _foundLat && _foundLon && _foundTime) {
            if (_currentTrack) {
                _currentTrack->points.append(*_currentPoint);
            }
            _currentPoint = NULL;
        }
    } else if (localName == "time") {
        _inTime = false;
    }
    return true;
}

bool 
GpxHandler::endDocument()
{
    return true;
}


