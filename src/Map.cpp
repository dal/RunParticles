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
_projection(Projection()),
_duration(0)
{
    
}

void
Map::draw(const ViewCtx &viewCtx, const TimeCtx &timeCtx)
{
    PassMap::iterator pass;
    LayerPtrList::iterator it;
    for (pass = _passes.begin(); pass != _passes.end(); pass++) {
        unsigned int thisPass = *pass;
        for (it = _layers.begin(); it != _layers.end(); it++) {
            (*it)->draw(thisPass, viewCtx, timeCtx);
        }
    }
}

bool
Map::addLayer(Layer *layer)
{
    LayerPtr layerPtr(layer);
    _layers.push_back(layerPtr);
    _layerMap.insert(std::pair<LayerId, LayerPtr>(layer->id(), layerPtr));
    // give the layer an opportunity to reproject
    layer->project(_projection);
    PassMap layerPasses = layer->passes();
    _passes.insert(layerPasses.begin(), layerPasses.end());
    _duration = (layer->duration() > _duration) ? layer->duration() : _duration;
    emit(signalLayerAdded());
    return true;
}

bool
Map::onMapClicked(const MapPoint &pt, const ViewCtx &viewCtx) const
{
    double dist = viewCtx.getResolution() * viewCtx.getResolution()
                  * SELECTION_TOLERANCE_PIXELS;
    LayerPtrList::const_iterator it;
    for (it = _layers.begin(); it != _layers.end(); it++) {
        if (pt.distanceSquared((*it)->position()) < dist) {
            emit(signalLayerClicked((*it)->id()));
            return true;
        }
    }
    return false;
}
