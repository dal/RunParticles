//
//  AddLayersCommand.hpp
//  RunParticles
//
//  Created by Doug Letterman on 5/13/16.
//
//

#ifndef AddLayersCommand_h
#define AddLayersCommand_h

#include <QUndoCommand>
#include <QString>

#include "Layer.h"
#include "Map.h"

struct LoadTrackLayerRequest;
class MainWindow;

class AddLayersCommand : public QUndoCommand
{
public:
    AddLayersCommand(MainWindow *mainWindow, QStringList layerPaths,
                     QUndoCommand *parent = Q_NULLPTR);
    void redo();
    void undo();
protected:
    MainWindow *_mainWindow;
    QList<std::shared_ptr<LoadTrackLayerRequest>> _loadRequests;
    QStringList _layerPaths;
    LayerId _sentinel;
};

#endif /* AddLayersCommand_h */
