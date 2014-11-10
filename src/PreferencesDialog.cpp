//
//  PreferencesDialog.cpp
//  RunParticles
//
//  Created by Doug Letterman on 11/10/14.
//
//

#include "PreferencesDialog.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent)
{
    _tableWidget = new QTableWidget(this);
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


