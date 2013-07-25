
#include "TcxHandler.h"
#include <time.h>

int parseTime(const char* timeCStr) {
    struct tm startTime;
    strptime(timeCStr, "%Y-%m-%dT%H:%M:%SZ", &startTime);
    return static_cast<int>(timegm(&startTime));
};

TcxHandler::TcxHandler(QList<Track*> *tracks) :
QXmlDefaultHandler(),
_tracks(tracks),
_currentTrack(NULL),
_depth(0),
_inTime(false),
_inLatitudeDegrees(false),
_inLongitudeDegrees(false),
_foundLat(false),
_foundLon(false),
_foundTime(false)
{
    
}

bool
TcxHandler::startDocument()
{
    _depth = 0;
    _inTime = false;
    _inLatitudeDegrees = false;
    _inLongitudeDegrees = false;
    _foundLat = false;
    _foundLon = false;
    return true;
}

bool 
TcxHandler::startElement(const QString & namespaceURI,
                      const QString & localName, 
                      const QString & qName, 
                      const QXmlAttributes & atts )
{
    if (localName == "Activity") {
        if (_depth != 0) {
            qWarning("Nested activites!");
        }
        int sportIdx = atts.index(QString(""), "Sport");
        if (sportIdx != -1) {
            QString mysport = atts.value(sportIdx);
            _currentTrack = new Track();
            _currentTrack->sport = mysport;
            _tracks->append(_currentTrack);
        } else {
            qWarning("Could not determine activity type");
        }
    } else if (localName == "Lap") {
        _depth += 1;
        // TODO get StartTme
        if (_startTime == 0) {
            int stIdx = atts.index(QString(""), "StartTime");
            if (stIdx != -1) {
                // startTime in format "2012-02-25T18:53:52Z"
                const char* cStartTime = atts.value(stIdx).toAscii().constData();
                _startTime = parseTime(cStartTime);
            }
        }
    } else if (localName == "Track") {
        _depth += 1;
    } else if (localName == "Trackpoint") {
        _depth += 1;
        _foundLat = false;
        _foundLon = false;
        _currentPoint = new TrackPoint();
    } else if (localName == "Time") {
        if (_currentPoint) {
            qDebug("In time");
            _inTime = true;
        }
    } else if (localName == "Position") {
        _depth += 1;
    } else if (localName == "LatitudeDegrees") {
        if (_currentPoint) {
            qDebug("In latitude");
            _inLatitudeDegrees = true;
        } else {
            qWarning("Found LatitudeDegrees outside TrackPoint");
        }
    } else if (localName == "LongitudeDegrees") {
        if (_currentPoint) {
            qDebug("In longitude");
            _inLongitudeDegrees = true;
        } else {
            qWarning("Found LongitudeDegrees outside TrackPoint");
        }
    }
    return true;
}

bool 
TcxHandler::characters ( const QString & ch )
{
    bool ok = false;
    if (_inLatitudeDegrees) {
        _currentPoint->pos.lat = ch.toDouble(&ok);
        if (ok) {
            _foundLat = true;
        } else {
            qWarning("Invalid latitude: %s", ch.toAscii().constData());
        }
    } else if (_inLongitudeDegrees) {
        _currentPoint->pos.lon = ch.toDouble(&ok);
        if (ok) {
            _foundLon = true;
        } else {
            qWarning("Invalid longitude: %s", ch.toAscii().constData());
        }
    } else if (_inTime && _currentPoint) {
        _currentPoint->time = parseTime(ch.toAscii().constData());
        _foundTime = true;
    }
    return true;
}

bool 
TcxHandler::endElement(const QString & namespaceURI,
                    const QString & localName, 
                    const QString & qName )
{
    if (localName == "Activity") {
        if (_currentTrack != NULL) {
            _currentTrack = NULL;
        }
        _depth -= 1;
    } else if (localName == "Lap") {
        _depth -= 1;
    } else if (localName == "Track") {
        _depth -= 1;
    } else if (localName == "Trackpoint") {
        _depth -= 1;
        if (_currentPoint && _foundLat && _foundLon && _foundTime) {
            if (_currentTrack) {
                _currentTrack->points.append(*_currentPoint);
            }
            _currentPoint = NULL;
        }
    } else if (localName == "Position") {
        _depth -= 1;
        if (!_foundLat) {
            qWarning("Position element was missing a valid latitude");
        } else if (!_foundLon) {
            qWarning("Position element was missing a valid longitude");
        }
    } else if (localName == "Time") {
        _inTime = false;
    } else if (localName == "LatitudeDegrees") {
        _inLatitudeDegrees = false;
    } else if (localName == "LongitudeDegrees") {
        _inLongitudeDegrees = false;
    }

    return true;
}

bool 
TcxHandler::endDocument()
{
    return true;
}

