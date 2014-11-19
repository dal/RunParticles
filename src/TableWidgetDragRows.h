#include <QTableWidget>
#include <QDialog>
#include <set>

// From https://bugreports.qt-project.org/browse/QTBUG-13873?page=com.atlassian.jira.plugin.system.issuetabpanels:all-tabpanel

class TableWidgetDragRows : public QTableWidget
{
public:
  TableWidgetDragRows(QWidget* parent = 0);
  void dropEvent(QDropEvent *event);
private:
  bool droppingOnItself(QDropEvent *event, const QModelIndex &index);
  bool dropOn(QDropEvent *event, int *dropRow, int *dropCol, QModelIndex *dropIndex);
  QAbstractItemView::DropIndicatorPosition position(const QPoint &pos, const QRect &rect, const QModelIndex &index) const;
  std::set<int> getSelectedRowsFast(QItemSelection const& selection);
};

class ExampleDialog : public QDialog
{
public:
  ExampleDialog(QWidget* parent=0, Qt::WindowFlags f=0);
};