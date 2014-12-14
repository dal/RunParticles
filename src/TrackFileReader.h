#ifndef TRACKFILEREADER_H
#define TRACKFILEREADER_H

#include <QFile>
#include <QList>
#include <QString>
#include <QObject>
#include <QThread>
#include <QMutex>

#include "Types.h"

typedef QPair<QString, QList<Track*>*> WorkPair;
typedef QList<WorkPair> WorkList;

class TrackFileReader;

class TrackFileReaderWorker : public QThread
{
    Q_OBJECT
    
public:
    TrackFileReaderWorker(TrackFileReader *parent=NULL);
    
    virtual ~TrackFileReaderWorker() { };
    
    void run();
    
    void read(const QString &path, QList<Track*> *tracks);

public slots:
    
    void cancel();
    
signals:
    void signalReady(const QString&, QList<Track*>*);
    
    void signalError(const QString&, const QString&);
    
    void signalUpdate(const QString& what, int total, int done);
    
    void signalDone();
    
protected:
    QMutex _inMutex;
    
    WorkList _input;
    
    TrackFileReader *_reader;
    
    int _workCount;
    
    bool _cancelRequested;
    
};

class TrackFileReader : public QObject
{
    
    Q_OBJECT
    
public:
    
    TrackFileReader(QObject *parent=NULL);
    
    virtual ~TrackFileReader();
    
    bool read(const QString &path,
              QList<Track*> *tracks,
              std::string *whyNot=NULL) const;
    
    void readDeferred(const QString &path, QList<Track*> *tracks);
    
signals:
    
    void signalReady(const QString&, QList<Track*>*);
    
    void signalError(const QString&, const QString&);
    
    void signalUpdate(const QString&, int, int);
    
    void signalDone();
    
protected:
    
    bool _readXml(QFile &theFile,
                 QList<Track*> *tracks,
                 std::string *whyNot=NULL) const;

    bool _readFit(const QString &path,
                 QList<Track*> *tracks,
                 std::string *whyNot=NULL) const;
    
    TrackFileReaderWorker *_worker;
};

#endif
