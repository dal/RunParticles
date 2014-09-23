#ifndef TRACKFILEREADER_H
#define TRACKFILEREADER_H

#include <QFile>
#include <QList>
#include <QString>
#include <QObject>

#include "Types.h"

class TrackFileReader : public QObject
{
    
    Q_OBJECT
    
public:
    TrackFileReader(QObject *parent=NULL);
    
    bool read(const QString &path,
              QList<Track*> *tracks,
              std::string *whyNot=NULL) const;
    
signals:
    
    void signalReady(const QString);
    
protected:
    bool _readXml(QFile &theFile,
                 QList<Track*> *tracks,
                 std::string *whyNot=NULL) const;

    bool _readFit(const QString &path,
                 QList<Track*> *tracks,
                 std::string *whyNot=NULL) const;
};

#endif
