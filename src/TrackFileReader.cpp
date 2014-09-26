
#include "TrackFileReader.h"

#include <stdio.h>

#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include <QtDebug>

#include "FitFileReader.h"
#include "XmlHandler.h"
#include "GpxHandler.h"
#include "TcxHandler.h"

TrackFileReaderWorker::TrackFileReaderWorker(TrackFileReader *parent) :
    QThread(parent),
    _reader(parent)
{
        
};
    
void
TrackFileReaderWorker::run() {
        while (true) {
            WorkPair work;
            _inMutex.lock();
            if (!_input.isEmpty()) {
                work = _input.first();
                _input.pop_front();
            } else {
                _inMutex.unlock();
                break;
            }
            _inMutex.unlock();
            std::string whyNot;
            if (!_reader->read(work.first, work.second, &whyNot)) {
                QString error = QString::fromStdString(whyNot);
                emit signalError(work.first, error);
            } else {
                emit signalReady(work.first, work.second);
            }
        }
    };
    
void
TrackFileReaderWorker::read(const QString &path, QList<Track*> *tracks) {
    QMutexLocker locker(&_inMutex);
    _input.append(WorkPair(path, tracks));
    start();
};

TrackFileReader::TrackFileReader(QObject *parent) :
    QObject(parent),
    _worker(new TrackFileReaderWorker(this))
{
    connect(_worker, &TrackFileReaderWorker::signalReady,
            this, &TrackFileReader::signalReady);
    connect(_worker, &TrackFileReaderWorker::signalError,
            this, &TrackFileReader::signalError);
}

bool
TrackFileReader::read(const QString &path, 
                      QList<Track*> *tracks,
                      std::string *whyNot) const
{
    bool success = false;
    QFile theFile(path);
    if (!theFile.exists()) {
        if (whyNot) {
            QString tmpWhyNot = QString("'%0' doesn't exist").arg(path);
            *whyNot = tmpWhyNot.toStdString();
        }
        success = false;
    } else if (FitFileReader::IsFitFile(path)) {
        success = _readFit(path, tracks, whyNot);
    } else {
        success = _readXml(theFile, tracks, whyNot);
    }
    return success;
}

bool
TrackFileReader::_readXml(QFile &theFile,
                          QList<Track*> *tracks,
                          std::string *whyNot) const
{
    XmlHandler xmlHandler;
    QXmlSimpleReader reader;
    reader.setContentHandler(&xmlHandler);
    reader.setErrorHandler(&xmlHandler);
    
    QXmlInputSource source(&theFile);
    
    reader.parse(&source, true /*incremental*/);
    while (reader.parseContinue()) { };
    TrackFileType fileType = xmlHandler.getType();
    switch (fileType) {
    case TrackFileType_Gpx:
        {
        GpxHandler gpxHandler(tracks);
        reader.setContentHandler(&gpxHandler);
        reader.setErrorHandler(&gpxHandler);
        break;
        }
    case TrackFileType_Tcx:
        {
        TcxHandler tcxHandler(tracks);
        reader.setContentHandler(&tcxHandler);
        reader.setErrorHandler(&tcxHandler);
        break;
        }
    default:
        {
        *whyNot = "Unrecognized file type";
        return false;
        break;
        }
    }
    source.reset();
    reader.parse(&source, true /*incremental*/);
    while (reader.parseContinue()) { };
    return true;
}

bool
TrackFileReader::_readFit(const QString &path,
                          QList<Track*> *tracks,
                          string *whyNot) const
{
    FitFileReader reader(tracks);
    return reader.readFile(path, whyNot);
}

