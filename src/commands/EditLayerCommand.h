//
//  EditLayerCommand.h
//  RunParticles
//
//  Created by Doug Letterman on 5/22/16.
//
//

#ifndef EditLayerCommand_h
#define EditLayerCommand_h

#include <QUndoCommand>
#include <QString>

#include "Layer.h"
#include "Map.h"

class MainWindow;

enum EditType {
    EditTypeName,
    EditTypeSport
};

class EditLayerCommand : public QUndoCommand
{
public:
    EditLayerCommand(MainWindow *mainWindow, LayerId layerId, EditType editType,
                     const QString value, QUndoCommand *parent = Q_NULLPTR);
    void redo();
    void undo();
protected:
    MainWindow *_mainWindow;
    LayerId _layerId;
    EditType _editType;
    QString _newValue;
    QString _oldValue;
};


#endif /* EditLayerCommand_h */
