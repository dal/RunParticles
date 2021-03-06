
#include "TrackFileReader.h"

#include <stdio.h>

#include <QXmlStreamReader>
#include <QtDebug>

#include "FitFileReader.h"
#include "GpxHandler.h"
#include "TcxHandler.h"

enum TrackFileType {
    TrackFileType_Unknown,
    TrackFileType_Gpx,
    TrackFileType_Tcx,
    TrackFileType_RunParticlesMap
};

TrackFileType getXmlType(QFile *file, QString *error)
{
    QXmlStreamReader xml(file);
    
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        QString name = xml.name().toString();
        if (xml.isStartElement()) {
            if (name == "TrainingCenterDatabase") {
                return TrackFileType_Tcx;
            } else if (name == "gpx") {
                return TrackFileType_Gpx;
            } else if (name == "RunParticlesMap") {
                return TrackFileType_RunParticlesMap;
            } else {
                return TrackFileType_Unknown;
            }
        }
    }
    if (xml.hasError()) {
        QString err = QString("Got an error at line %0 column %1: '%2'")
                      .arg(xml.lineNumber()).arg(xml.columnNumber())
                      .arg(xml.errorString());
        qWarning() << err;
        error->swap(err);
    }
    return TrackFileType_Unknown;
};

TrackFileReaderWorker::TrackFileReaderWorker(TrackFileReader *parent) :
    QThread(parent),
    _reader(parent),
    _workCount(0),
    _cancelRequested(false)
{
        
};
    
void
TrackFileReaderWorker::run() {
    while (!_cancelRequested) {
        DeferredLoadRequest *req = nullptr;
        {
            QMutexLocker locker(&_inMutex);
            if (!_requests.isEmpty()) {
                req = _requests.first();
                _requests.pop_front();
                QString what = QString("Parsing %0").arg(req->path);
                emit signalUpdate(what, _workCount,
                                  _workCount - _requests.length());
            } else {
                _workCount = 0;
                emit signalDone();
                break;
            }
        }
        if (!req)
            return;
        std::string whyNot;
        if (!_reader->read(req->path, req->tracks, &whyNot)) {
            req->error = QString::fromStdString(whyNot);
            req->status.store(RequestStatusError);
            emit signalError(req);
        } else {
            req->status.store(RequestStatusReady);
            emit signalReady(req);
        }
    }
};
    
void
TrackFileReaderWorker::read(DeferredLoadRequest *request) {
    QMutexLocker locker(&_inMutex);
    _cancelRequested = false;
    if (_requests.length() == 0)
        _workCount = 0;
    _requests.append(request);
    _workCount++;
    start();
};

void
TrackFileReaderWorker::cancel()
{
    _cancelRequested = true;
    wait();
    _requests.clear();
}

TrackFileReader::TrackFileReader(QObject *parent) :
    QObject(parent),
    _worker(new TrackFileReaderWorker(this))
{
    connect(_worker, &TrackFileReaderWorker::signalReady,
            this, &TrackFileReader::signalReady);
    connect(_worker, &TrackFileReaderWorker::signalError,
            this, &TrackFileReader::signalError);
    connect(_worker, SIGNAL(signalUpdate(const QString&, int, int)),
            SIGNAL(signalUpdate(const QString&, int, int)));
    connect(_worker, SIGNAL(signalDone()), SIGNAL(signalDone()));
}

TrackFileReader::~TrackFileReader()
{
    _worker->cancel();
}

bool
TrackFileReader::read(const QString &path, 
                      QList<Track*> &tracks,
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
    
    /* Make sure all the tracks return their source file path */
    if (success) {
        foreach(Track *track, tracks)
            track->sourceFilePath = path;
    }
    
    return success;
}

DeferredLoadRequest*
TrackFileReader::readDeferred(const QString &path)
{
    DeferredLoadRequest *request = new DeferredLoadRequest();
    request->path = path;
    _worker->read(request);
    return request;
}

bool
TrackFileReader::_readXml(QFile &theFile,
                          QList<Track*> &tracks,
                          std::string *whyNot) const
{
    QString error;
    theFile.open(QIODevice::ReadOnly);
    TrackFileType fileType = getXmlType(&theFile, &error);
    theFile.seek(0);
    switch (fileType) {
    case TrackFileType_Gpx:
        {
        GpxHandler gpxHandler(&tracks);
        bool ok = gpxHandler.parse(&theFile);
        if (!ok && whyNot) {
            whyNot->assign(gpxHandler.getError().toStdString());
        }
        return ok;
        break;
        }
    case TrackFileType_Tcx:
        {
        TcxHandler tcxHandler(&tracks);
        bool ok = tcxHandler.parse(&theFile);
        if (!ok && whyNot) {
            whyNot->assign(tcxHandler.getError().toStdString());
        }
        return ok;
        break;
        }
    default:
        {
        if (whyNot) {
            if (!error.isEmpty()) {
                *whyNot = error.toStdString();
            } else {
                *whyNot = "Unrecognized file type";
            }
        }
        return false;
        break;
        }
    }
    return true;
}

bool
TrackFileReader::_readFit(const QString &path,
                          QList<Track*> &tracks,
                          string *whyNot) const
{
    FitFileReader reader(&tracks);
    return reader.readFile(path, whyNot);
}

