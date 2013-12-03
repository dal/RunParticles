
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
    : QTreeWidget(parent)
{
    setColumnCount(LayerListColCount);
    QStringList columns = QStringList() << "Visible" << "Name"
                                        << "Sport" << "Start" << "Duration";
    setHeaderLabels(columns);
    setSortingEnabled(true);
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
    item->setData(LayerListColStartTime, LayerListDateTimeRole,
                  QVariant(layer->startTime()));
    item->setData(LayerListColDuration, LayerListNumericRole,
                  QVariant(layer->duration()));
}
