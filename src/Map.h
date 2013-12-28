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
#include "TimeCtx.h"
#include "ViewCtx.h"

#include <map>
#include <vector>

typedef std::map<LayerId, Layer*> LayerMap;

class Map : public QObject
{
    Q_OBJECT
    
public:
    Map(QObject *parent=0);
    
    void draw();
    
    bool addLayer(Layer*);
    
    ViewCtx* getViewCtx() const { return _viewCtx; }
    
    TimeCtx* getTimeCtx() const { return _timeCtx; }
    
    int getDuration() const { return _duration; }
    
    Layer* getLayer(const LayerId id) { return _layerMap[id]; }
    
private:
    
    std::vector<Layer*> _layers;
    
    LayerMap _layerMap;
    
    TimeCtx *_timeCtx;
    
    ViewCtx *_viewCtx;
    
    unsigned int _numPasses;
    
    unsigned int _duration;
    
signals:
    
    void signalLayerAdded();
    
};


#endif