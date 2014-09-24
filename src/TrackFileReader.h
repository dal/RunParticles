#ifndef TRACKFILEREADER_H
#define TRACKFILEREADER_H

#include <QFile>
#include <QList>
#include <QString>
#include <QObject>

#include "Types.h"

class TrackFileReaderWorker;

class TrackFileReader : public QObject
{
    
    Q_OBJECT
    
public:
    TrackFileReader(QObject *parent=NULL);
    
    bool read(const QString &path,
              QList<Track*> *tracks,
              std::string *whyNot=NULL) const;
    
    bool readDeferred(const QString &path, QList<Track*> *tracks);
    
signals:
    
    void signalReady(const QString&, QList<Track*>*);
    
    void signalError(const QString&, const QString&);
    
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
