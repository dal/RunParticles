//
//  AddLayersCommand.hpp
//  RunParticles
//
//  Created by Doug Letterman on 5/13/16.
//
//

#ifndef AddLayersCommand_hpp
#define AddLayersCommand_hpp

#include <QUndoCommand>
#include <QString>

#include "Layer.h"
#include "Map.h"

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
    QStringList _layerPaths;
    LayerId _sentinel;
};

#endif /* AddLayersCommand_hpp */
