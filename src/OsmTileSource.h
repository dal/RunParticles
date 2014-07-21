/*
 *  OsmTileSource.h
 *  RunParticles
 *
 *  Created by Doug Letterman on 6/30/14.
 *  Copyright 2014 Doug Letterman. All rights reserved.
 *
 */

#ifndef OSMTILESOURCE_H
#define OSMTILESOURCE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "Singleton.h"

#include "boost/shared_ptr.hpp"

#include "cinder/Surface.h"
#include "math.h"
#include <unordered_map>
#include <map>

struct OsmIndex {
    OsmIndex(unsigned int x, unsigned int y, unsigned int z) : x(x), y(y), z(z) {};
    unsigned int x, y, z;
};

struct OsmTile {
    OsmIndex index;
    std::shared_ptr<unsigned int*> _imageData;
    cinder::Surface *surface;
};

template<typename T>
struct OsmHasher
{
    std::size_t operator()(const T& t) const
    {
        unsigned long offset = 0;
        for (int i=1; i < t.z; i++) {
            offset += pow(2, 2*i);
        }
        int edge = pow(2, t.z);
        return std::size_t(offset + t.x * edge + t.y);
    }
};

typedef std::shared_ptr<OsmTile*> OsmTileRef;

typedef std::unordered_map<OsmIndex, OsmTileRef, OsmHasher<OsmIndex>> OsmTileMap;

typedef std::map<time_t, OsmIndex> OsmTileTimeMap;

class OsmTileSource : public QObject
{
    Q_OBJECT
    
public:
    
    enum {
        memCacheSize = 128
    };
    
    void getTile(int x, int y, int z);
    
signals:
    
    void tileReady(int x, int y, int z);
    
public slots:
    
    void onRequestFinished(QNetworkReply *reply);
    
protected:
    
    void _requestTile(const OsmIndex index) const;
    
    OsmTileMap _memoryTileCache;
    
    OsmTileTimeMap _memoryTileHistory;
    
};

#endif