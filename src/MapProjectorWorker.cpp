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
    _projection(projection),
    _cancelRequested(false)
{
}
    
MapProjectorWorker::~MapProjectorWorker()
{
}
    
void
MapProjectorWorker::run()
{
    while (!_cancelRequested) {
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
    _cancelRequested = false;
    QMutexLocker locker(&_inMutex);
    _input.append(layer);
    start();
}

void
MapProjectorWorker::project(QList<LayerPtr> layers)
{
    _cancelRequested = false;
    QMutexLocker locker(&_inMutex);
    _input << layers;
    start();
}

int
MapProjectorWorker::queue() const
{
    return _input.length();
}

void
MapProjectorWorker::cancel()
{
    _cancelRequested = true;
    wait();
    _input.clear();
}
