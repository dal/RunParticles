//
//  RemoveLayerCommand.cpp
//  RunParticles
//
//  Created by Doug Letterman on 5/8/16.
//
//

#include "RemoveLayersCommand.h"

#include "MainWindow.h"

RemoveLayerCommand::RemoveLayerCommand(MainWindow* mainWindow,
                                       QList<LayerId> layerIds,
                                       QUndoCommand *parent)
    : QUndoCommand::QUndoCommand(parent),
    _layerIds(layerIds),
    _mainWindow(mainWindow)
{
    
}

void
RemoveLayerCommand::redo()
{
    // Store the layer for undo()
    LayerId myId;
    foreach(myId, _layerIds) {
        LayerPtr thisLayer = _mainWindow->getLayerPtr(myId);
        if (thisLayer)
            _layers.push_back(thisLayer);
    }
    _mainWindow->removeLayers(_layerIds);
}

void
RemoveLayerCommand::undo()
{
    for (auto i=_layers.begin(); i != _layers.end(); ++i) {
        _mainWindow->addLayer(i->get());
    }
}
