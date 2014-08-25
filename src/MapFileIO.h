//
//  MapFileWriter.h
//  RunParticles
//
//  Created by Doug Letterman on 8/19/14.
//
//

#ifndef __RunParticles__MapFileIO__
#define __RunParticles__MapFileIO__

#include <QString>
#include <QStringList>

#include "Map.h"

class MapFileIO : public QObject
{
    Q_OBJECT
    
public:
    MapFileIO(QObject *parent=NULL);
    
    bool writeMapFile();
    
    bool loadMapFile(char **whyNot=NULL);
    
    void addTrackFile(const QString &filename);
    
    void setFilename(const QString &filename);
    
    void clear();
    
    QString getFilename() const { return _filename; };
    
    QStringList getTrackFiles() const { return _trackFiles; };
    
    bool isDirty() const { return _dirty; }
    
protected:
    
    bool _dirty;
    
    QStringList _trackFiles;
    
    QString _filename;
    
};

#endif /* defined(__RunParticles__IO__) */
