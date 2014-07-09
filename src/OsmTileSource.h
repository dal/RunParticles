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

#include "cinder/Surface.h"
#include "math.h"
#include <unordered_map>

struct OsmIndex {
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

typedef std::unordered_map<OsmIndex, OsmHasher<OsmIndex>> OsmTileMap;

class OsmTileSource : public QObject
{
    Q_OBJECT
    
public:
    
    void getTile(int x, int y, int z);
    
signals:
    
    void tileReady(int x, int y, int z);
    
public slots:
    
    void onRequestFinished(QNetworkReply *reply);
    
protected:
    
    OsmTileMap tiles;
    
};

#endif
