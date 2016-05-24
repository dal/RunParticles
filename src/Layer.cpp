#include "Layer.h"

LayerId Layer::_gid = 0;

bool
Layer::ephemeral() const {
    return false;
}

bool
Layer::userEditable() const {
    return false;
}