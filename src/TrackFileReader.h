#ifndef TRACKFILEREADER_H
#define TRACKFILEREADER_H

#include <QFile>
#include <QList>
#include <QString>

#include "Types.h"

class TrackFileReader 
{
public:
    TrackFileReader();
    
    bool read(const QString &path,
              QList<Track*> *tracks,
              char **whyNot=NULL) const;
    
protected:
    bool _readXml(QFile &theFile,
                 QList<Track*> *tracks,
                 char **whyNot=NULL) const;

    bool _readFit(const QString &path,
                 QList<Track*> *tracks,
                 char **whyNot=NULL) const;
};

#endif
