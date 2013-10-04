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

#include "Layer.h"
#include "TimeCtx.h"
#include "ViewCtx.h"

#include <vector>

class Map 
{
public:
    Map(TimeCtx*);
    
    void update();
    
    void draw();
    
    bool addLayer(Layer*);
    
    ViewCtx* getViewCtx() const { return _viewCtx; }
    
private:
    
    std::vector<Layer*> _layers;
    
    TimeCtx *_timeCtx;
    
    ViewCtx *_viewCtx;
    
};


#endif