#include "GpxHandler.h"
#include <time.h>

#include <QtDebug>
#include <QXmlStreamReader>

#include "Util.h"

GpxHandler::GpxHandler(QList<Track*> *tracks) :
_tracks(tracks),
_currentTrack(NULL),
_depth(0),
_inPoint(false),
_foundLat(false),
_foundLon(false),
_foundTime(false)
{
    
}

bool
GpxHandler::parse(QFile *gpxFile)
{
    gpxFile->seek(0);
    QXmlStreamReader xml(gpxFile);

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        QString name = xml.name().toString();
        if (xml.isStartElement()) {
            if (name == "trk") {
                if (_depth != 0) {
                    qWarning() << "Nested tracks at line " << xml.lineNumber()
                    << " column " << xml.columnNumber();;
                }
                _currentTrack = new Track();
                _currentTrack->sport = "Unknown (gpx)";
                _tracks->append(_currentTrack);
            } else if (name == "name" && _currentTrack) {
                _currentTrack->name = xml.readElementText();
            } else if (name == "trkseg") {
                _depth += 1;
            } else if (name == "trkpt") {
                _depth += 1;
                _foundLat = false;
                _foundLon = false;
                _inPoint = true;
                bool ok = false;
                QXmlStreamAttributes attrs = xml.attributes();
                if (attrs.hasAttribute("lat")) {
                    QString lat = attrs.value("lat").toString();
                    _currentPoint.pos.y = lat.toDouble(&ok);
                    if (ok) {
                        _foundLat = true;
                    } else {
                        qWarning() << "Invalid trk element latitude: '"
                        << lat << "' at line " << xml.lineNumber() << " column "
                        << xml.columnNumber();
                    }
                } else {
                    qWarning() << "trk element at line " << xml.lineNumber() <<
                    "column " << xml.columnNumber() <<
                    "was missing a latitude attribute";
                }
                
                if (attrs.hasAttribute("lon")) {
                    QString lon = attrs.value("lon").toString();
                    _currentPoint.pos.x = lon.toDouble(&ok);
                    if (ok) {
                        _foundLon = true;
                    } else {
                        qWarning() << "Invalid trk element longitude: '"
                        << lon << "' at line " << xml.lineNumber() << " column "
                        << xml.columnNumber();
                    }
                } else {
                    qWarning() << "trk element at line " << xml.lineNumber() <<
                    "column " << xml.columnNumber() <<
                    "was missing a longitude attribute";
                }
            } else if (name == "time") {
                if (_inPoint) {
                    QByteArray theTime = xml.readElementText().toLocal8Bit();
                    _currentPoint.time = Util::parseTime(theTime.constData());
                    _foundTime = true;
                }
                    
            }
        } else if (xml.isEndElement()) {
            if (name == "trk") {
                _depth -= 1;
                _currentTrack = NULL;
            } else if (name == "trkseg") {
                _depth -= 1;
            } else if (name == "trkpt") {
                _depth -= 1;
                if (_currentTrack && _inPoint && _foundLat && _foundLon
                    && _foundTime)
                {
                    /* push a copy of our point into the list */
                    _currentTrack->points.push_back(TrackPoint(_currentPoint));
                }
                _inPoint = false;
            }
        }
    }
    if (xml.hasError()) {
        _error = QString("Got an error at line %0 column %1: '%2'").arg(
                         xml.lineNumber()).arg(xml.columnNumber())
                         .arg(xml.errorString());
        qWarning() << _error;
        return false;
    }
    return true;
}


