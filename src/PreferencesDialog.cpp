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
#include <QVBoxLayout>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Preferences");
    _colorDialog = new QColorDialog(this);
    _tableWidget = new QTableWidget(this);
    _tableWidget->setColumnCount(TrackColorTable_Num_Cols);
    QStringList headerLabels;
    headerLabels << "Pattern" << "Color";
    _tableWidget->setHorizontalHeaderLabels(headerLabels);
    QGroupBox *tableBox = new QGroupBox("Track color rules", this);
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
    TrackColorPrefs trackColors = settings->getTrackColorPrefs();
    _tableWidget->clearContents();
    _tableWidget->setRowCount(trackColors.prefs.count());
    for (int row=0; row < trackColors.prefs.length(); row++) {
        TrackColorPref myPref = trackColors.prefs[row];
        QTableWidgetItem *pItem = new QTableWidgetItem(myPref.pattern);
        _tableWidget->setItem(row, TrackColorTable_Pattern_Col, pItem);
        QTableWidgetItem *cItem = new QTableWidgetItem();
        cItem->setBackground(QBrush(myPref.color));
        cItem->setFlags(Qt::ItemIsEnabled);
        _tableWidget->setItem(row, TrackColorTable_Color_Col, cItem);
    }
}

void
PreferencesDialog::slotCellDoubleClicked(int row, int col)
{
    if (col != TrackColorTable_Color_Col)
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


