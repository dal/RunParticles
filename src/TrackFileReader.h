#ifndef TRACKFILEREADER_H
#define TRACKFILEREADER_H

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

};

#endif
