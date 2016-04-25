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

#include "BoundingBox.h"
#include "Map.h"
#include "Types.h"

class MapFileIO : public QObject
{
    Q_OBJECT
    
public:
    MapFileIO(QObject *parent=NULL);
    
    // Deprecated: Use exportMap
    bool writeMapFile(bool relativePaths = false);
    
    bool exportMap(const LayerPtrList& layers);
    
    bool loadMapFile(char **whyNot=NULL);
    
    bool importMapFile(QList<Track*> &tracks, char **whyNot=NULL);
    
    void addTrackFile(const QString &filename);
    
    void setFilename(const QString &filename);
    
    void clear();
    
    QString getFilename() const { return _filename; };
    
    QStringList getTrackFiles() const { return _trackFiles; };
    
    LonLatBox getViewArea() const { return _viewArea; };
    
    void setViewArea(const LonLatBox &bbox);
    
    bool isDirty() const { return _dirty; }
    
    QString relativeizePath(const QString &path);
    
    QString absoluteizePath(const QString &path);
    
protected:
    
    bool _dirty;
    
    QStringList _trackFiles;
    
    QString _filename;
    
    LonLatBox _viewArea;
    
private:
    
    static const QByteArray _fileHeaderByteArray;
    
    static const quint16 _fileVersion;
    
};

#endif /* defined(__RunParticles__IO__) */
