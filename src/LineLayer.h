#ifndef __LINE_LAYER_H__
#define __LINE_LAYER_H__

#include "Layer.h"
#include "Types.h"

class LineLayer : public Layer
{
public:
    LineLayer();
    
    virtual ~LineLayer();
    
    virtual void draw(const ViewCtx*, const TimeCtx*);
};

#endif;
