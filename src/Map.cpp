/*
 *  Map.cpp
 *  RunParticles
 *
 *  Created by Doug Letterman on 2/18/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#include "Map.h"

Map::Map(TimeCtx *timeCtx)
: _timeCtx(timeCtx)
{
    
}

void
Map::update()
{
    // TODO
}

void
Map::draw()
{
    const vector::iterator i;
    for (i = _layers.begin(); i != _layers.end(); i++) {
        // TODO *i.draw(viewctx);
    }
}

bool
Map::addLayer(const Layer *layer)
{
    _layers.push_back(layer);
    return true;
}