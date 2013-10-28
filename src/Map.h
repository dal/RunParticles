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

#include <vector>

class Map : public QObject
{
    Q_OBJECT
    
public:
    Map(QObject *parent=0);
    
    void update();
    
    void draw();
    
    bool addLayer(Layer*);
    
    ViewCtx* getViewCtx() const { return _viewCtx; }
    
    TimeCtx* getTimeCtx() const { return _timeCtx; }
    
    int getDuration() const { return _duration; }
    
private:
    
    std::vector<Layer*> _layers;
    
    TimeCtx *_timeCtx;
    
    ViewCtx *_viewCtx;
    
    unsigned int _numPasses;
    
    unsigned int _duration;
    
signals:
    
    void layerAdded();
    
};


#endif