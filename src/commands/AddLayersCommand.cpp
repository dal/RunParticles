//
//  AddLayersCommand.cpp
//  RunParticles
//
//  Created by Doug Letterman on 5/13/16.
//
//

#include "AddLayersCommand.h"
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
    foreach(path, _layerPaths) {
        _mainWindow->loadTrackFile(path);
    }
}

void
AddLayersCommand::undo()
{
    LayerPtrList layers = _mainWindow->getLayers();
    LayerPtr thisLayer;
    QList<LayerId> layerIds;
    foreach(thisLayer, layers) {
        if (_layerPaths.contains(thisLayer->sourceFilePath())
            && thisLayer->id() >= _sentinel) {
            layerIds.append(thisLayer->id());
        }
    }
    _mainWindow->removeLayers(layerIds);
}
