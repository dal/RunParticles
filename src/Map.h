/*
 *  Map.h
 *  RunParticles
 *
 *  Created by Doug Letterman on 2/18/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include "Layer.h"
#include "Projection.h"
#include "TimeCtx.h"
#include "ViewCtx.h"

#include <map>
#include <vector>

#define SELECTION_TOLERANCE_PIXELS 16.

typedef std::shared_ptr<Layer> LayerPtr;

typedef std::vector<LayerPtr> LayerPtrList;

typedef std::map<LayerId, LayerPtr> LayerMap;

class Map;

class MapProjectorWorker : public QThread
{
    Q_OBJECT
    
public:
    MapProjectorWorker(Map *parent=NULL);
    
    virtual ~MapProjectorWorker() { };
    
    void run();
    
    void project(LayerPtr);
    
signals:
    void signalReady(LayerPtr);
    
    void signalError(LayerId, const QString&);
    
protected:
    QMutex _inMutex;
    
    QList<LayerPtr> _input;
    
    Map *_map;
    
};

class Map : public QObject
{
    Q_OBJECT
    
public:
    Map(QObject *parent=0);
    
    void draw(const ViewCtx&, const TimeCtx&);
    
    bool addLayer(Layer*);
    
    int getDuration() const { return _duration; }
    
    Layer* getLayer(const LayerId id) { return _layerMap[id].get(); }
    
    const Projection getProjection() const { return _projection; }
    
    LayerPtrList getLayers() const;
    
    void clearLayers();
    
public slots:
    
    bool onMapClicked(const MapPoint &pt, const ViewCtx &viewCtx) const;
    
signals:
    
    void layerUpdated();
    
private:
    
    const Projection _projection;
    
    LayerPtrList _layers;
    
    LayerMap _layerMap;
    
    unsigned int _duration;
    
    PassMap _passes;
    
signals:
    
    void signalLayerAdded(LayerId);
    
    void signalLayerClicked(LayerId) const;
    
};


#endif