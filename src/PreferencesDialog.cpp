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
    _tableWidget = new TableWidgetDragRows(this);
    _tableWidget->setColumnCount(TrackStyleTable_Num_Cols);
    QStringList headerLabels;
    headerLabels << "Pattern" << "Color" << "Width";
    _tableWidget->setHorizontalHeaderLabels(headerLabels);
    QGroupBox *tableBox = new QGroupBox("Track style rules", this);
    QVBoxLayout *tableBoxLayout = new QVBoxLayout();
    tableBoxLayout->addWidget(_tableWidget);
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
    connect(_saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void
PreferencesDialog::loadSettings(Settings *settings)
{
    TrackStyleRules trackRules = settings->getTrackStyleRules();
    _tableWidget->clearContents();
    _tableWidget->setRowCount(trackRules.prefs.count());
    for (int row=0; row < trackRules.prefs.length(); row++) {
        TrackStyleRule myPref = trackRules.prefs[row];
        QTableWidgetItem *pItem = new QTableWidgetItem(myPref.pattern);
        _tableWidget->setItem(row, TrackStyleTable_Pattern_Col, pItem);
        QTableWidgetItem *cItem = new QTableWidgetItem();
        cItem->setBackground(QBrush(myPref.color));
        cItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        _tableWidget->setItem(row, TrackStyleTable_Color_Col, cItem);
        QLineEdit *widthLineEdit = new QLineEdit(_tableWidget);
        widthLineEdit->setFrame(false);
        widthLineEdit->setValidator(new QIntValidator(widthLineEdit));
        widthLineEdit->setText(QString("%0").arg(myPref.width));
        _tableWidget->setCellWidget(row, TrackStyleTable_Width_Col,
                                    widthLineEdit);
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


