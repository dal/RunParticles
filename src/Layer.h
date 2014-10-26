//
//  Layer.h
//  RunParticles
//
//  Created by Doug Letterman on 3/30/13.
//
//

#ifndef RunParticles_Layer_h
#define RunParticles_Layer_h

#include <set>
#include <QString>
#include <QDateTime>
#include <QObject>

#include "BoundingBox.h"
#include "Projection.h"
#include "Types.h"

class ViewCtx;
class TimeCtx;

// Represents a single map layer

typedef unsigned int LayerId;

class Layer;

typedef std::shared_ptr<Layer> LayerPtr;

Q_DECLARE_METATYPE(LayerPtr);

class Layer : public QObject
{
    
    Q_OBJECT
    
public:
    
    Layer() : QObject(), _id(Layer::getNextId()), _visible(true) {};
    
    virtual ~Layer() {};
    
    LayerId id() const { return _id; }
    
    virtual QString name() const = 0;
    
    virtual QString sourceFilePath() const { return QString(); };
    
    virtual QString sport() const = 0;
    
    virtual QDateTime startTime() const = 0;
    
    virtual PassMap passes() const = 0;
    
    virtual unsigned int duration() const { return 0; }
    
    virtual bool visible() const { return _visible; }
    
    virtual void setVisible(bool newVisible) { _visible = newVisible; };
    
    virtual void project(const Projection&) = 0;
    
    virtual void draw(uint pass, const ViewCtx&, const TimeCtx&) = 0;
    
    // projected bounds
    virtual BoundingBox getBoundingBox() const = 0;
    
    virtual MapPoint position() const = 0;
    
    virtual bool ephemeral() const;
    
signals:
    
    void layerUpdated();
    
protected:
    static LayerId getNextId() { return _gid++; }
    
    static LayerId _gid;
    
    LayerId _id;
    
    bool _visible;

};

#endif
