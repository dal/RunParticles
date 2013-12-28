
#include "LayerListWidget.h"

#include <QString>

#include "Util.h"

#define DATE_FMT "MMM d, yyyy h:mm:ss ap"

bool
LayerListWidgetItem::operator<(const QTreeWidgetItem & other) const
{
    int sortCol = treeWidget()->sortColumn();
    if (sortCol == LayerListColStartTime) {
        QDateTime myTime = data(sortCol, LayerListDateTimeRole).toDateTime();
        QDateTime otherTime = other.data(sortCol,
                                         LayerListDateTimeRole).toDateTime();
        return myTime < otherTime;
    } else if (sortCol == LayerListColDuration) {
        unsigned int myDuration = data(sortCol, LayerListNumericRole).toUInt();
        unsigned int otherDuration = other.data(sortCol,
                                                LayerListNumericRole).toUInt();
        return myDuration < otherDuration;
    } else {
        return text(sortCol) < other.text(sortCol);
    }
}

LayerListWidget::LayerListWidget(QWidget *parent)
    : QTreeWidget(parent),
    _frameLayerAction(new QAction("Frame Selected Layers", this))
{
    setColumnCount(LayerListColCount);
    QStringList columns = QStringList() << "Visible" << "Name"
                                        << "Sport" << "Start" << "Duration";
    setHeaderLabels(columns);
    setSortingEnabled(true);
    addAction(_frameLayerAction);
    connect(_frameLayerAction, SIGNAL(triggered()),
            this, SLOT(onFrameLayersSelected()));
    setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(onSelectionChanged()));
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
    item->setData(LayerListColName, LayerListLayerIdRole,
                  QVariant(layer->id()));
    item->setData(LayerListColStartTime, LayerListDateTimeRole,
                  QVariant(layer->startTime()));
    item->setData(LayerListColDuration, LayerListNumericRole,
                  QVariant(layer->duration()));
}

QList<LayerId>
LayerListWidget::selectedLayerIds() const
{
    QList<unsigned int> selectedIds;
    QList<QTreeWidgetItem *> items = selectedItems();
    QTreeWidgetItem *thisItem;
    foreach(thisItem, items) {
        selectedIds.push_back(
                              thisItem->data(LayerListColName, LayerListLayerIdRole).toUInt());
    }
    return selectedIds;
}

void
LayerListWidget::onSelectionChanged()
{
    emit signalLayersSelected(selectedLayerIds());
}

void
LayerListWidget::onFrameLayersSelected()
{
    emit signalFrameLayers(selectedLayerIds());
}

