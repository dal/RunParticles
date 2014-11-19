#include "TableWidgetDragRows.h"
#include <QtGui/QDropEvent>
#include <set>

TableWidgetDragRows::TableWidgetDragRows(QWidget* parent):
QTableWidget(parent)
{
  // Enable drag and drop to rearrange rows
  setDragEnabled(true);
  setAcceptDrops(true);
  viewport()->setAcceptDrops(true);
  setDragDropOverwriteMode(false);
  setDropIndicatorShown(true);
  setDragDropMode(QAbstractItemView::InternalMove);
}


void TableWidgetDragRows::dropEvent(QDropEvent *event)
{
  if (event->source() == this && (event->dropAction() == Qt::MoveAction ||
    dragDropMode() == QAbstractItemView::InternalMove)) 
  {
    QModelIndex topIndex;
    int col = -1;
    int row = -1;
    if (this->dropOn(event, &row, &col, &topIndex)) 
    {
      std::set<int> selRows = getSelectedRowsFast(selectionModel()->selection());
      int top = *(selRows.begin());
      int dropRow = topIndex.row();
      if(dropRow == -1)
      {
        dropRow = rowCount();
      }
      int offset = dropRow - top;

      // insert new rows
      std::set<int>::const_iterator it;
      for(it = selRows.begin(); it != selRows.end(); ++it)
      {
        int r = *it + offset ;
        r = qMin(r, rowCount());
        r = qMax(r, 0);
        insertRow(r);
      }

      // copy data
      // note: row numbers may have changed after inserts, hence refresh selRows
      selRows = getSelectedRowsFast(selectionModel()->selection());
      top = *(selRows.begin());
      offset = dropRow - top;
      for(it = selRows.begin(); it != selRows.end(); ++it)
      {
        int r = *it + offset ;
        r = qMin(r, rowCount());
        r = qMax(r, 0);
        for(int c = 0; c < columnCount(); ++c)
        {
          QTableWidgetItem *source = new QTableWidgetItem(*item(*it, c));
          setItem(r, c, source);
        }
      }

      // delete selected rows. 
      std::set<int>::const_reverse_iterator rit;
      for(rit = selRows.rbegin(); rit != selRows.rend(); ++rit)
      {
        removeRow(*rit);
      }

      event->accept();
    }
  }
  else
  {
    QTableView::dropEvent(event);
  }

}



std::set<int> TableWidgetDragRows::getSelectedRowsFast(QItemSelection const& selection)
{
  std::set<int> selRows;

  Q_FOREACH(QItemSelectionRange const& range, selection)
  {
    if (range.isValid() && range.model())
    {
      for (int row = range.top(); row <= range.bottom(); ++row)
      {
        for (int column = range.left(); column <= range.right(); ++column)
        {
          QModelIndex index = range.model()->index(row, column, range.parent());
          Qt::ItemFlags flags = range.model()->flags(index);
          if ((flags & Qt::ItemIsSelectable) && (flags & Qt::ItemIsEnabled))
          {
            if (selRows.insert(index.row()).second == false)
            {
              break; // Insert failed, we assume we've already got the entry
            }
          }
        }
      }
    }
  }
  return selRows;
}


/*!
Copied from Qt

Return true if this is a move from ourself and \a index is a child of the selection that
is being moved.
*/
bool TableWidgetDragRows::droppingOnItself(QDropEvent *event, const QModelIndex &index)
{
  Qt::DropAction dropAction = event->dropAction();
  if (this->dragDropMode() == QAbstractItemView::InternalMove)
  {
    dropAction = Qt::MoveAction;
  }
  if (event->source() == this
    && event->possibleActions() & Qt::MoveAction
    && dropAction == Qt::MoveAction) 
  {
    QModelIndexList selectedIndexes = this->selectedIndexes();
    QModelIndex child = index;
    while (child.isValid() && child != rootIndex()) 
    {
      if (selectedIndexes.contains(child))
        return true;
      child = child.parent();
    }
  }
  return false;
}

/*!
Copied from Qt

If the event hasn't already been accepted, determines the index to drop on.

if (row == -1 && col == -1)
// append to this drop index
else
// place at row, col in drop index

If it returns true a drop can be done, and dropRow, dropCol and dropIndex reflects the position of the drop.
*/
bool TableWidgetDragRows::dropOn(QDropEvent *event, int *dropRow, int *dropCol, QModelIndex *dropIndex)
{
  if (event->isAccepted())
    return false;

  QModelIndex index;
  // rootIndex() (i.e. the viewport) might be a valid index
  if (viewport()->rect().contains(event->pos())) 
  {
    index = this->indexAt(event->pos());
    if (!index.isValid() || !this->visualRect(index).contains(event->pos()))
    {
      index = rootIndex();
    }
  }

  // If we are allowed to do the drop
  if (model()->supportedDropActions() & event->dropAction()) 
  {
    int row = -1;
    int col = -1;
    QAbstractItemView::DropIndicatorPosition dropIndicatorPosition;
    if (index != rootIndex()) 
    {
      dropIndicatorPosition = position(event->pos(), this->visualRect(index), index);
      switch (dropIndicatorPosition) 
      {
      case QAbstractItemView::AboveItem:
        row = index.row();
        col = index.column();
        index = index.parent();
        break;
      case QAbstractItemView::BelowItem:
        row = index.row() + 1;
        col = index.column();
        index = index.parent();
        break;
      case QAbstractItemView::OnItem:
      case QAbstractItemView::OnViewport:
        break;
      }
    } else 
    {
      dropIndicatorPosition = QAbstractItemView::OnViewport;
    }
    *dropIndex = index;
    *dropRow = row;
    *dropCol = col;
    if (!droppingOnItself(event, index))
      return true;
  }
  return false;
}

QAbstractItemView::DropIndicatorPosition
TableWidgetDragRows::position(const QPoint &pos, const QRect &rect, const QModelIndex &index) const
{
  QAbstractItemView::DropIndicatorPosition r = QAbstractItemView::OnViewport;

  const int margin = 2;
  if (pos.y() - rect.top() < margin) 
  {
    r = QAbstractItemView::AboveItem;
  } 
  else if (rect.bottom() - pos.y() < margin) 
  {
    r = QAbstractItemView::BelowItem;
  } 
  else if (rect.contains(pos, true)) 
  {
    r = QAbstractItemView::OnItem;
  }


  if (r == QAbstractItemView::OnItem && (!(model()->flags(index) & Qt::ItemIsDropEnabled)))
  {
    r = pos.y() < rect.center().y() ? QAbstractItemView::AboveItem : QAbstractItemView::BelowItem;
  }

  return r;
}


ExampleDialog::ExampleDialog(QWidget* parent/*=0*/, Qt::WindowFlags f/*=0*/)
: QDialog(parent, f)
{
  TableWidgetDragRows *tw = new TableWidgetDragRows(this);

  // populate table widget with sample data
  tw->setRowCount(5);
  tw->setColumnCount(1);
  int row = 0;
  tw->setItem(row++, 0, new QTableWidgetItem(tr("A is for apple")));
  tw->setItem(row++, 0, new QTableWidgetItem(tr("B is for boy")));
  tw->setItem(row++, 0, new QTableWidgetItem(tr("C is for cat")));
  tw->setItem(row++, 0, new QTableWidgetItem(tr("D is for dog")));
  tw->setItem(row++, 0, new QTableWidgetItem(tr("E is for elephant")));
}