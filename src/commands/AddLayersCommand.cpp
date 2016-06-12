//
//  AddLayersCommand.cpp
//  RunParticles
//
//  Created by Doug Letterman on 5/13/16.
//
//

#include "AddLayersCommand.h"

#include "Layer.h"
#include "MainWindow.h"

AddLayersCommand::AddLayersCommand(MainWindow *mainWindow,
                                   QStringList layerPaths,
                                   QUndoCommand *parent)
    : QUndoCommand(parent),
    _mainWindow(mainWindow),
    _layerPaths(layerPaths),
    _sentinel(0)
{
    
}

void
AddLayersCommand::redo()
{
    QString path;
    _sentinel = Layer::getLastId();
    if (_loadRequests.empty()) {
        foreach(path, _layerPaths) {
            LoadTrackLayerRequestPtr myReq = _mainWindow->loadTrackFile(path);
            _loadRequests.append(myReq);
        }
    } else {
        foreach(auto myReq, _loadRequests) {
            if (myReq->status == RequestStatusReady) {
                foreach (LayerPtr myLayerPtr, myReq->layers) {
                    _mainWindow->addLayer(myLayerPtr);
                }
            }
        }
    }
}

void
AddLayersCommand::undo()
{
    foreach(auto myReq, _loadRequests) {
        if (myReq->status == RequestStatusReady) {
            QList<LayerId> myLayerIds;
            foreach (LayerPtr myLayer, myReq->layers) {
                myLayerIds.append(myLayer->id());
            }
            if (!myLayerIds.empty())
                _mainWindow->removeLayers(myLayerIds);
        }
    }
}
