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

#include <map>
#include <vector>

#define SELECTION_TOLERANCE_PIXELS 5.0

typedef std::shared_ptr<Layer> LayerPtr;

typedef std::vector<LayerPtr> LayerPtrList;

typedef std::map<LayerId, LayerPtr> LayerMap;

struct OsmTile {
    unsigned int x, y, z;
};

template<>
class OsmTileHash<OsmTile>
{
public:
    std::size_t operator()(const OsmTile& t) const 
    {
        unsigned int edge = pow(2, t.z)
        unsigned int last = t.z > 0 ? pow(2, 2*(t.z-1)) : 0;
        return std::size_t(last + t.x * edge + t.y);
    }
};

class OsmTileSource : public QObject
{
    Q_OBJECT
    
public:
    
    void getTile(int x, int y, int z);
    
signals:
    
    void tileReady(int x, int y, int x);
    
public slots:
    
    void onRequestFinished(QNetworkReply *reply);
    
protected:
    
    
    
}

#endif
