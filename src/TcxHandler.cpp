
#include "TcxHandler.h"

#include <QXmlStreamReader>
#include <QtDebug>
#include <QStringRef>

#include "Util.h"

TcxHandler::TcxHandler(QList<Track*> *tracks) :
_tracks(tracks),
_currentTrack(NULL),
_depth(0),
_foundLat(false),
_foundLon(false),
_foundTime(false)
{
    
}

bool
TcxHandler::parse(QFile *tcxFile)
{
    QXmlStreamReader xml(tcxFile);
    
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        QString name = xml.name().toString();
        if (xml.isStartElement()) {
            if (name == "Activity") {
                if (_depth != 0) {
                    qWarning() << "Nested activites at line "
                        << xml.lineNumber() << " column " << xml.columnNumber();
                }
                _currentTrack = new Track();
                QXmlStreamAttributes attrs = xml.attributes();
                if (attrs.hasAttribute("Sport")) {
                    _currentTrack->sport = attrs.value("Sport").toString();
                } else {
                    qWarning() << "Could not determine activity type at line "
                        << xml.lineNumber() << " column " << xml.columnNumber();
                }
                _tracks->append(_currentTrack);
            } else if (name == "Lap") {
                _depth += 1;
            } else if (name == "Track") {
                _depth += 1;
            } else if (name == "Id") {
                QXmlStreamReader::TokenType token = xml.readNext();
                if (_currentTrack && token == QXmlStreamReader::Characters)
                    _currentTrack->name = xml.text().toString();
            } else if (name == "Trackpoint") {
                if (_currentTrack) {
                    _depth += 1;
                    _foundLat = false;
                    _foundLon = false;
                    _inPoint = true;
                }
            } else if (name == "Time") {
                if (_inPoint) {
                    QXmlStreamReader::TokenType token = xml.readNext();
                    if (token == QXmlStreamReader::Characters) {
                        QByteArray theTime = xml.text().toLocal8Bit();
                        _currentPoint.time = Util::parseTime(theTime.constData());
                        _foundTime = true;
                    }
                }
            } else if (name == "LatitudeDegrees") {
                if (_inPoint) {
                    QXmlStreamReader::TokenType token = xml.readNext();
                    if (token == QXmlStreamReader::Characters) {
                        bool ok = false;
                        _currentPoint.pos.y = xml.text().toDouble(&ok);
                        if (ok) {
                            _foundLat = true;
                        } else {
                            qWarning() << "Invalid latitude: '" << xml.text()
                                << "' at line " << xml.lineNumber()
                                << " column " << xml.columnNumber();
                        }
                    }
                }
            } else if (name == "LongitudeDegrees") {
                if (_inPoint) {
                    QXmlStreamReader::TokenType token = xml.readNext();
                    if (token == QXmlStreamReader::Characters) {
                        bool ok = false;
                        _currentPoint.pos.x = xml.text().toDouble(&ok);
                        if (ok) {
                            _foundLon = true;
                        } else {
                            qWarning() << "Invalid longitude: '" << xml.text()
                            << "' at line " << xml.lineNumber()
                            << " column " << xml.columnNumber();
                        }
                    }
                }
            }
        } else if (xml.isEndElement()) {
            if (name == "Activity") {
                _depth -= 1;
                _currentTrack = NULL;
            } else if (name == "Lap" || name == "Track" || name == "Id") {
                _depth -= 1;
            } else if (name == "Trackpoint") {
                _depth -= 1;
                if (_inPoint && _foundLat && _foundLon && _foundTime) {
                    if (_currentTrack) {
                        _currentTrack->points.push_back(_currentPoint);
                    }
                    _inPoint = false;
                }
            } else if (name == "Position") {
                _depth -= 1;
                if (!_foundLat) {
                    qWarning() << "Position element was missing a valid "
                         "latitude at line " << xml.lineNumber() << " column "
                        << xml.columnNumber();
                }
                if (!_foundLon) {
                    qWarning() << "Position element was missing a valid "
                        "longitude at line " << xml.lineNumber() << " column "
                        << xml.columnNumber();
                }
            }
        }
    }
    if (xml.hasError()) {
        _error = QString("Got an error at line %0 column %1: '%2'")
                .arg(xml.lineNumber()).arg(xml.columnNumber())
                .arg(xml.errorString());
        qWarning() << _error;
        return false;
    }
    return true;

}
