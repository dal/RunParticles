//
//  MapProjectorWorker.cpp
//  RunParticles
//
//  Created by Doug Letterman on 10/25/14.
//
//

#include "MapProjectorWorker.h"

#include "Map.h"
    
MapProjectorWorker::MapProjectorWorker(const Projection *projection,
                                       QObject *parent)
    : QThread(parent),
    _projection(projection)
{
}
    
MapProjectorWorker::~MapProjectorWorker()
{
}
    
void
MapProjectorWorker::run()
{
    while (true) {
        LayerPtr thisLayer;
        _inMutex.lock();
        if (!_input.isEmpty()) {
            thisLayer = _input.first();
            _input.pop_front();
        } else {
            _inMutex.unlock();
            break;
        }
        _inMutex.unlock();
        std::string whyNot;
        thisLayer->project(*_projection);
        emit signalReady(thisLayer);
    }
}
    
void
MapProjectorWorker::project(LayerPtr layer)
{
    QMutexLocker locker(&_inMutex);
    _input.append(layer);
    start();
}
