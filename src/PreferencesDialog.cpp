//
//  PreferencesDialog.cpp
//  RunParticles
//
//  Created by Doug Letterman on 11/10/14.
//
//

#include "PreferencesDialog.h"

#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Preferences");
    _colorDialog = new QColorDialog(this);
    _removeRuleAction = new QAction("Remove rule", this);
    _tableWidget = new TableWidgetDragRows(this);
    _tableWidget->addAction(_removeRuleAction);
    _tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    _tableWidget->setColumnCount(TrackStyleTable_Num_Cols);
    QStringList headerLabels;
    headerLabels << "Pattern" << "Color" << "Width";
    _tableWidget->setHorizontalHeaderLabels(headerLabels);
    QGroupBox *tableBox = new QGroupBox("Track style rules", this);
    _addRuleButton = new QPushButton("Add rule");
    QVBoxLayout *tableBoxLayout = new QVBoxLayout();
    tableBoxLayout->addWidget(_tableWidget);
    tableBoxLayout->addWidget(_addRuleButton);
    tableBox->setLayout(tableBoxLayout);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tableBox);
    _saveButton = new QPushButton("Save Preferences", this);
    _cancelButton = new QPushButton("Cancel", this);
    QHBoxLayout *bottomRow = new QHBoxLayout();
    bottomRow->addStretch();
    bottomRow->addWidget(_cancelButton);
    bottomRow->addWidget(_saveButton);
    layout->addLayout(bottomRow);
    
    connect(_tableWidget, &QTableWidget::cellDoubleClicked,
            this, &PreferencesDialog::slotCellDoubleClicked);
    connect(_addRuleButton, &QPushButton::clicked,
            this, &PreferencesDialog::slotAddRuleButtonClicked);
    connect(_removeRuleAction, &QAction::triggered,
            this, &PreferencesDialog::slotRemoveRule);
    connect(_saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void
PreferencesDialog::loadSettings(Settings *settings)
{
    TrackStyleRules trackRules = settings->getTrackStyleRules();
    _tableWidget->clearContents();
    _tableWidget->setRowCount(0);
    for (int row=0; row < trackRules.prefs.length(); row++) {
        TrackStyleRule myPref = trackRules.prefs[row];
        _addRuleRow(myPref.pattern, myPref.color, myPref.width);
    }
}

TrackStyleRules
PreferencesDialog::getTrackStyleRules() const
{
    TrackStyleRules rules;
    for (int row=0; row < _tableWidget->rowCount(); row++) {
        TrackStyleRule myRule;
        QTableWidgetItem *rItem = _tableWidget->item(row,
                                                   TrackStyleTable_Pattern_Col);
        myRule.pattern = rItem->text();
        QTableWidgetItem *cItem = _tableWidget->item(row,
                                                     TrackStyleTable_Color_Col);
        myRule.color = cItem->backgroundColor();
        QLineEdit *widthLineEdit =
            dynamic_cast<QLineEdit*>(_tableWidget->cellWidget(row,
                                                     TrackStyleTable_Width_Col));
        if (widthLineEdit)
            myRule.width = widthLineEdit->text().toUInt();
        rules.prefs.append(myRule);
    }
    return rules;
}

void
PreferencesDialog::slotCellDoubleClicked(int row, int col)
{
    if (col != TrackStyleTable_Color_Col)
        return;
    QTableWidgetItem *item = _tableWidget->item(row, col);
    QBrush bgBrush = item->background();
    QColor color = bgBrush.color();
    _colorDialog->setCurrentColor(color);
    if (_colorDialog->exec() == QDialog::Accepted) {
        bgBrush.setColor(_colorDialog->currentColor());
        item->setBackground(bgBrush);
    }
}

void
PreferencesDialog::slotAddRuleButtonClicked()
{
    QString rule("*");
    QColor color(255, 0, 0);
    _addRuleRow(rule, color, 1);
}

void
PreferencesDialog::slotRemoveRule()
{
    int row = _tableWidget->currentRow();
    _tableWidget->removeRow(row);
}

void
PreferencesDialog::_addRuleRow(const QString &pattern, const QColor &color,
                               int width)
{
    int row = _tableWidget->rowCount();
    _tableWidget->setRowCount(row+1);
    QTableWidgetItem *pItem = new QTableWidgetItem(pattern);
    _tableWidget->setItem(row, TrackStyleTable_Pattern_Col, pItem);
    QTableWidgetItem *cItem = new QTableWidgetItem();
    cItem->setBackground(QBrush(color));
    cItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    _tableWidget->setItem(row, TrackStyleTable_Color_Col, cItem);
    QLineEdit *widthLineEdit = new QLineEdit(_tableWidget);
    widthLineEdit->setFrame(false);
    widthLineEdit->setValidator(new QIntValidator(widthLineEdit));
    widthLineEdit->setText(QString("%0").arg(width));
    _tableWidget->setCellWidget(row, TrackStyleTable_Width_Col,
                            widthLineEdit);
}

