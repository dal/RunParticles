//
//  MapProjectorWorker.h
//  RunParticles
//
//  Created by Doug Letterman on 10/25/14.
//
//

#ifndef __RunParticles__MapProjectorWorker__
#define __RunParticles__MapProjectorWorker__

#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#include "Layer.h"

class Map;

class Projection;

class MapProjectorWorker : public QThread
{
    Q_OBJECT
    
public:
    
    MapProjectorWorker(const Projection *projection, QObject *parent);
    
    virtual ~MapProjectorWorker();
    
    void run();
    
    void project(LayerPtr layer);
    
    void project(QList<LayerPtr> layers);
    
    int queue() const;
    
public slots:
    
    void cancel();
    
signals:
    void signalReady(LayerPtr);
    
protected:
    QMutex _inMutex;
    
    QList<LayerPtr>_input;
    
    const Projection* _projection;
    
    bool _cancelRequested;
    
};

#endif /* defined(__RunParticles__MapProjectorWorker__) */
