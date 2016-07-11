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
    
    /* Deprecated: Use exportMap
     * Legacy map files are simply a list of paths to .FIT, .GPX, or .TCX
     * activity files that RunParticles finds and loads separately.
     */
    bool writeLegacyMapFile(bool relativePaths = false);
    
    /* Export the map data to a custom binary format optimized for use in
     * RunParticles. Loading map data from such a file is much faster than 
     * loading tracks from .FIT, .TCX, or .GPX files.
     */
    bool exportMap(const LayerPtrList& layers);
    
    /* Deprectated: Uee importMapFile. This method is preserved here so that
     * old-style XML mapFiles may still be loaded.
     */
    bool loadLegacyMapFile(char **whyNot=NULL);
    
    /* Load map data from the custom binary Runparticles map file format.
     *
     */
    bool loadMapFile(QList<Track*> &tracks, char **whyNot=NULL);
    
    void setFilename(const QString &filename);
    
    void clear();
    
    QString getFilename() const { return _filename; };
    
    QStringList getTrackFiles() const { return _trackFiles; };
    
    LonLatBox getViewArea() const { return _viewArea; };
    
    void setViewArea(const LonLatBox &bbox);
    
    bool isLegacyMapFile() const { return _isLegacyMapFile; }
    
    QString relativeizePath(const QString &path);
    
    QString absoluteizePath(const QString &path);
    
protected:
    
    QStringList _trackFiles;
    
    QString _filename;
    
    LonLatBox _viewArea;
    
    bool _isLegacyMapFile;
    
private:
    
    static const QByteArray _fileHeaderByteArray;
    
    static const quint16 _fileVersion;
    
};

#endif /* defined(__RunParticles__IO__) */
