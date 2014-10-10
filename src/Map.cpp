/*
 *  Map.cpp
 *  RunParticles
 *
 *  Created by Doug Letterman on 2/18/13.
 *  Copyright 2013 Doug Letterman. All rights reserved.
 *
 */

#include "Map.h"

#include <QDebug>

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
    connect(layer, SIGNAL(layerUpdated()), SIGNAL(layerUpdated()));
    emit(signalLayerAdded(layer->id()));
    return true;
}

LayerPtrList
Map::getLayers() const
{
    return _layers;
}

void
Map::clearLayers()
{
    LayerPtrList::iterator it;
    for (it = _layers.begin(); it != _layers.end(); it++) {
        (*it)->disconnect();
    }
    _passes.clear();
    _layers.clear();
    _layerMap.clear();
}

bool
Map::onMapClicked(const MapPoint &pt, const ViewCtx &viewCtx) const
{
    double limit = pow(viewCtx.getResolution() * SELECTION_TOLERANCE_PIXELS, 2);
    LayerId closestId = 0;
    double closestDistance = 0.;
    bool found = false;
    LayerPtrList::const_iterator it;
    for (it = _layers.begin(); it != _layers.end(); it++) {
        if ((*it)->visible()) {
            double myDist = pt.distanceSquared((*it)->position());
            if (myDist < limit && (!found || myDist < closestDistance)) {
                closestDistance = myDist;
                closestId = (*it)->id();
                found = true;
                qDebug() << "selected " << (*it)->id();
            }
        }
    }
    if (found)
        emit(signalLayerClicked(closestId));
    return found;
}

