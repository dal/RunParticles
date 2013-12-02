
#include "LayerListWidget.h"

#include <QString>
#include <QTreeWidgetItem>

#include "Util.h"

#define DATE_FMT "MMM d, yyyy h:mm:ss ap"

LayerListWidget::LayerListWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setColumnCount(LayerListColCount);
    QStringList columns = QStringList() << "Visible" << "Name"
                                        << "Sport" << "Start" << "Duration";
    setHeaderLabels(columns);
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
    QTreeWidgetItem *item = new QTreeWidgetItem(this, colData);
}
