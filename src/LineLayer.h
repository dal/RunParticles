#ifndef __LINE_LAYER_H__
#define __LINE_LAYER_H__

#include "Layer.h"

class LineLayer : public Layer
{
    LineLayer();
    
    virtual ~LineLayer();
    
    virtual void draw(const ViewCtx&);
};

#endif;
