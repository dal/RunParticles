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

#include "Layer.h"
#include "Projection.h"
#include "TimeCtx.h"
#include "ViewCtx.h"

#include <map>
#include <vector>

#define SELECTION_TOLERANCE_PIXELS 16.

typedef std::vector<LayerPtr> LayerPtrList;

typedef std::map<LayerId, LayerPtr> LayerMap;

class MapProjectorWorker;

class Map : public QObject
{
    Q_OBJECT
    
public:
    Map(QObject *parent=0);
    
    virtual ~Map();
    
    void draw(const ViewCtx&, const TimeCtx&);
    
    bool addLayer(Layer*);
    
    bool addLayer(LayerPtr layer);
    
    QList<LayerPtr> addLayers(QList<Layer*>);
    
    void removeLayers(const QList<LayerId> &layerIds);
    
    int getDuration() const { return _duration; }
    
    int getLayerCount() const;
    
    Layer* getLayer(const LayerId id);
    
    LayerPtr getLayerPtr(const LayerId id);
    
    const Projection getProjection() const { return _projection; }
    
    LayerPtrList getLayers() const;
    
    void clearLayers();
    
    /* Returns true if all queued layers have been projected and can be 
     displayed */
    bool ready() const;
    
public slots:
    
    bool onMapClicked(const MapPoint &pt, const ViewCtx &viewCtx) const;
    
protected slots:
    
    void _onLayerProjected(LayerPtr layer);
    
signals:
    
    void layerUpdated();
    
    void signalLayersRemoved(QList<LayerId>);
    
private:
    
    const Projection _projection;
    
    LayerPtrList _layers;
    
    LayerMap _layerMap;
    
    unsigned int _duration;
    
    PassMap _passes;
    
    MapProjectorWorker* _projector;
    
signals:
    
    void signalLayerAdded(LayerId);
    
    void signalLayerClicked(LayerId) const;
    
};


#endif