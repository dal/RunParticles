#ifndef LAYERLISTWIDGET_H
#define LAYERLISTWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "Layer.h"

enum LayerListColumns
{
    LayerListColVisible,
    LayerListColName,
    LayerListColSport,
    LayerListColStartTime,
    LayerListColDuration,
    LayerListColCount
};

enum LayerListRoles
{
    LayerListNumericRole = Qt::UserRole,
    LayerListDateTimeRole
};

class LayerListWidgetItem : public QTreeWidgetItem
{
public:
    LayerListWidgetItem(QTreeWidget *tree) : QTreeWidgetItem(tree)  {}
    LayerListWidgetItem(QTreeWidget * parent, const QStringList & strings)
        : QTreeWidgetItem (parent,strings)  {}
    
    bool operator<(const QTreeWidgetItem & other) const;
};

class LayerListWidget : public QTreeWidget
{
    
    Q_OBJECT
    
public:
    LayerListWidget(QWidget *parent=0);
    ~LayerListWidget();
    void addLayer(Layer *layer);
};

#endif
