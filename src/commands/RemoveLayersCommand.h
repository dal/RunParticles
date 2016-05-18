//
//  RemoveLayerCommand.hpp
//  RunParticles
//
//  Created by Doug Letterman on 5/8/16.
//
//

#ifndef RemoveLayerCommand_h
#define RemoveLayerCommand_h

#include <QUndoCommand>

#include "Map.h"

class MainWindow;

class RemoveLayerCommand : public QUndoCommand
{
public:
    RemoveLayerCommand(MainWindow *mainWindow, QList<LayerId> layerIds,
                       QUndoCommand *parent = Q_NULLPTR);
    void redo();
    void undo();
protected:
    QList<LayerId> _layerIds;
    MainWindow *_mainWindow;
    LayerPtrList _layers;
};

#endif /* RemoveLayerCommand_hpp */
