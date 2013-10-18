/*
 *  Map.cpp
 *  RunParticles
 *
 *  Created by Doug Letterman on 2/18/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#include "Map.h"

Map::Map(QObject *parent)
: QObject(parent),
_timeCtx(new TimeCtx()),
_viewCtx(new ViewCtx())
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
    std::vector<Layer*>::iterator i;
    for (i = _layers.begin(); i != _layers.end(); i++) {
        (*i)->draw(_viewCtx, _timeCtx);
    }
}

bool
Map::addLayer(Layer *layer)
{
    _layers.push_back(layer);
    emit(layerAdded());
    return true;
}
