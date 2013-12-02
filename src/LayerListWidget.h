#ifndef LAYERLISTWIDGET_H
#define LAYERLISTWIDGET_H

#include <QTreeWidget>

#include "Layer.h"

enum LayerListColumns
{
    LayerListColVisible,
    LayerListColName,
    LayerListSport,
    LayerListStart,
    LayerListColDuration,
    LayerListColCount
};

class LayerListWidget : public QTreeWidget
{
public:
    LayerListWidget(QWidget *parent=0);
    virtual ~LayerListWidget();
    void addLayer(Layer *layer);
};

#endif
