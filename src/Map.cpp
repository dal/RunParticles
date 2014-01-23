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
_viewCtx(new ViewCtx()),
_numPasses(1),
_duration(0)
{
    
}

void
Map::draw()
{
    std::vector<Layer*>::iterator it;
    for (uint pass=0; pass < _numPasses; pass++){
        for (it = _layers.begin(); it != _layers.end(); it++) {
            (*it)->draw(pass, _viewCtx, _timeCtx);
        }
    }
}

bool
Map::addLayer(Layer *layer)
{
    _layers.push_back(layer);
    _layerMap.insert(std::pair<LayerId, Layer*>(layer->id(), layer));
    // give the layer an opportunity to reproject
    layer->project(_viewCtx);
    _numPasses = (layer->passes() > _numPasses) ? layer->passes() : _numPasses;
    _duration = (layer->duration() > _duration) ? layer->duration() : _duration;
    emit(signalLayerAdded());
    return true;
}

bool
Map::onMapClicked(const MapPoint &pt) const
{
    double dist = _viewCtx->getResolution() * _viewCtx->getResolution()
                  * SELECTION_TOLERANCE_PIXELS;
    std::vector<Layer*>::const_iterator it;
    for (it = _layers.begin(); it != _layers.end(); it++) {
        if (pt.distanceSquared((*it)->position()) < dist) {
            emit(signalLayerClicked((*it)->id()));
            return true;
        }
    }
    return false;
}
