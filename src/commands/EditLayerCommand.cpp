//
//  EditLayerCommand.cpp
//  RunParticles
//
//  Created by Doug Letterman on 5/22/16.
//
//

#include "EditLayerCommand.h"
#include "MainWindow.h"

EditLayerCommand::EditLayerCommand(MainWindow *mainWindow, LayerId layerId,
                                   EditType editType, const QString value,
                                   QUndoCommand *parent)
    : QUndoCommand(parent),
    _mainWindow(mainWindow),
    _layerId(layerId),
    _editType(editType),
    _newValue(value)
{
    
}

void
EditLayerCommand::redo()
{
    LayerPtr thisLayer = _mainWindow->getLayerPtr(_layerId);
    if (!thisLayer)
        return;
    switch (_editType)
    {
        case EditTypeName:
        {
            _oldValue = thisLayer->name();
            _mainWindow->setLayerName(_layerId, _newValue);
            break;
        }
        case EditTypeSport:
        {
            _oldValue = thisLayer->sport();
            _mainWindow->setLayerSport(_layerId, _newValue);
            break;
        }
        default:
        {
            break;
        }
    };
}

void
EditLayerCommand::undo()
{
    switch (_editType)
    {
        case EditTypeName:
        {
            _mainWindow->setLayerName(_layerId, _oldValue);
            break;
        }
        case EditTypeSport:
        {
            _mainWindow->setLayerSport(_layerId, _oldValue);
            break;
        }
        default:
        {
            break;
        }
    };
}