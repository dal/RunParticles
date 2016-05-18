
#include "LayerListWidget.h"

#include <QString>
#include <QSet>

#include "Util.h"

#define DATE_FMT "MMM d, yyyy h:mm:ss ap"

void
LayerListWidgetItem::setData(int column, int role, const QVariant & value)
{
    Qt::CheckState state = checkState(column);
    QTreeWidgetItem::setData(column, role, value);
    if (role == Qt::CheckStateRole && state != checkState(column)) {
        LayerListWidget *myTree = dynamic_cast<LayerListWidget*>(treeWidget());
        if (myTree)
            myTree->itemChecked(this, column);
    }
}

bool
LayerListWidgetItem::operator<(const QTreeWidgetItem & other) const
{
    int sortCol = treeWidget()->sortColumn();
    if (sortCol == LayerListWidget::ColumnStartTime) {
        QDateTime myTime = data(sortCol,
                                LayerListWidget::DateTimeRole).toDateTime();
        QDateTime otherTime = other.data(sortCol,
                                    LayerListWidget::DateTimeRole).toDateTime();
        return myTime < otherTime;
    } else if (sortCol == LayerListWidget::ColumnDuration) {
        unsigned int myDuration = data(sortCol,
                                       LayerListWidget::NumericRole).toUInt();
        unsigned int otherDuration = other.data(sortCol,
                                         LayerListWidget::NumericRole).toUInt();
        return myDuration < otherDuration;
    } else {
        return text(sortCol) < other.text(sortCol);
    }
}

LayerListWidget::LayerListWidget(QWidget *parent)
    : QTreeWidget(parent),
    _frameLayerAction(new QAction("Frame selected layers", this)),
    _lockViewAction(new QAction("Lock View to layer", this)),
    _showLayersAction(new QAction("Show layers", this)),
    _hideLayersAction(new QAction("Hide layers", this)),
    _removeLayersAction(new QAction("Remove layers", this)),
    _inLayerAdd(false)
{
    setObjectName("LayerListWidget");
    setWindowTitle("Layer list");
    setColumnCount(ColumnCount);
    QStringList columns = QStringList() << "Visible" << "Name"
                                        << "Sport" << "Start" << "Duration";
    setHeaderLabels(columns);
    setSortingEnabled(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    addAction(_frameLayerAction);
    addAction(_lockViewAction);
    addAction(_showLayersAction);
    addAction(_hideLayersAction);
    addAction(_removeLayersAction);
    connect(_frameLayerAction, &QAction::triggered,
            this, &LayerListWidget::onFrameLayersSelected);
    connect(_lockViewAction, &QAction::triggered,
            this, &LayerListWidget::onLockViewSelected);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(this, &LayerListWidget::itemSelectionChanged,
            this, &LayerListWidget::onSelectionChanged);
    connect(_showLayersAction, &QAction::triggered,
            this, &LayerListWidget::onShowLayersSelected);
    connect(_hideLayersAction, &QAction::triggered,
            this, &LayerListWidget::onHideLayersSelected);
    connect(_removeLayersAction, &QAction::triggered,
            this, &LayerListWidget::onRemoveLayersSelected);
    sortByColumn(ColumnStartTime, Qt::AscendingOrder);
    resizeColumnToContents(ColumnVisible);
}

LayerListWidget::~LayerListWidget()
{
    // do nothing
}

void
LayerListWidget::addLayer(Layer *layer)
{
    QString duration;
    QStringList colData;
    colData << "" 
            << layer->name()
            << layer->sport()
            << layer->startTime().toString(DATE_FMT) 
            << Util::secondsToString(layer->duration());
    LayerListWidgetItem *item = new LayerListWidgetItem(this, colData);
    _inLayerAdd = true;
    item->setCheckState(ColumnVisible, ( layer->visible()
                                        ? Qt::Checked : Qt::Unchecked));
    item->setData(ColumnName, LayerIdRole, QVariant(layer->id()));
    item->setData(ColumnStartTime, DateTimeRole, QVariant(layer->startTime()));
    item->setData(ColumnDuration, NumericRole, QVariant(layer->duration()));
    _inLayerAdd = false;
}

void
LayerListWidget::removeLayers(const QList<LayerId> &layerIds)
{
    QSet<LayerId> layerIdsSet = QSet<LayerId>().fromList(layerIds);
    QTreeWidgetItemIterator it(this);
    while(*it) {
        LayerId myLayerId = (*it)->data(ColumnName, LayerIdRole).toUInt();
        if (layerIdsSet.remove(myLayerId)) {
            delete (*it);
        }
        ++it;
    }
}

QList<LayerId>
LayerListWidget::selectedLayerIds() const
{
    QList<unsigned int> selectedIds;
    QList<QTreeWidgetItem *> items = selectedItems();
    QTreeWidgetItem *thisItem;
    foreach(thisItem, items) {
        selectedIds.push_back(thisItem->data(ColumnName, LayerIdRole).toUInt());
    }
    return selectedIds;
}

void
LayerListWidget::itemChecked(LayerListWidgetItem *which, int column)
{
    if (column == ColumnVisible) {
        LayerId layerId = which->data(ColumnName, LayerIdRole).toUInt();
        if (!_inLayerAdd)
            emit signalLayerVisibilityChanged(layerId,
                                      which->checkState(column) == Qt::Checked);
    }
}

void
LayerListWidget::slotSetSelectedLayers(QList<LayerId> layerIds)
{
    clearSelection();
    QTreeWidgetItem *root = invisibleRootItem();
    for (int i = 0; i < root->childCount(); i++) {
        QTreeWidgetItem *item = root->child(i);
        LayerId thisLayerId = item->data(ColumnName, LayerIdRole).toUInt();
        if (layerIds.contains(thisLayerId)) {
            item->setSelected(true);
            if (layerIds.length() == 1)
                setCurrentItem(item);
        }
    }
}

void
LayerListWidget::onSelectionChanged()
{
    emit signalLayerSelectionChanged(selectedLayerIds());
}

void
LayerListWidget::onFrameLayersSelected()
{
    emit signalFrameLayers(selectedLayerIds());
}

void LayerListWidget::onShowLayersSelected()
{
    QTreeWidgetItem *myItem;
    foreach(myItem, selectedItems()) {
        myItem->setCheckState(ColumnVisible, Qt::Checked);
    }
}

void LayerListWidget::onHideLayersSelected()
{
    QTreeWidgetItem *myItem;
    foreach(myItem, selectedItems()) {
        myItem->setCheckState(ColumnVisible, Qt::Unchecked);
    }
}

void
LayerListWidget::onLockViewSelected()
{
    QList<LayerId> layerIds = selectedLayerIds();
    if (!layerIds.empty())
        emit signalLockViewToLayer(layerIds.last());
}

void
LayerListWidget::onRemoveLayersSelected()
{
    QList<LayerId> layerIds = selectedLayerIds();
    if (!layerIds.empty())
        emit signalRemoveLayersSelected(layerIds);
}
