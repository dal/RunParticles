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
    _tableWidget = new QTableWidget(this);
    _tableWidget->setColumnCount(TrackColorTable_Num_Cols);
    QStringList headerLabels;
    headerLabels << "Pattern" << "Color";
    _tableWidget->setHorizontalHeaderLabels(headerLabels);
    QGroupBox *tableBox = new QGroupBox("Track colors", this);
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
        qDebug() << myPref.pattern;
        _tableWidget->setItem(row, TrackColorTable_Pattern_Col, pItem);
        QTableWidgetItem *cItem = new QTableWidgetItem();
        cItem->setBackground(QBrush(myPref.color));
        _tableWidget->setItem(row, TrackColorTable_Color_Col, cItem);
    }
}


