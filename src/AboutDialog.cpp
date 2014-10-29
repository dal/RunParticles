//
//  AboutDialog.cpp
//  RunParticles
//
//  Created by Doug Letterman on 10/28/14.
//
//

#include "AboutDialog.h"

#include <QApplication>
#include <QGridLayout>
#include <QPalette>
#include <QString>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    _icon = new QLabel(this);
    _icon->setPixmap(QPixmap(":/appIcon128"));
    
    _textEdit = new QTextBrowser(this);
    _textEdit->setSource(QUrl("qrc:about.html"));
    _textEdit->setFrameStyle(QFrame::NoFrame);
    _textEdit->setReadOnly(true);
    _textEdit->setAlignment(Qt::AlignCenter);
    _textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _textEdit->setOpenExternalLinks(true);
    QPalette p;
    p.setColor(QPalette::Base, p.color(QPalette::Background));
    _textEdit->setPalette(p);
    
    _version = new QLabel(QString("Version %0").arg(qApp->applicationVersion()));
    
    _closeButton = new QPushButton("Close", this);
    
    /* layout */
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(_icon, 0, 0, 1, 1, Qt::AlignCenter);
    layout->addWidget(_textEdit, 1, 0);
    layout->addWidget(_version, 2, 0, 1, 1);
    layout->addWidget(_closeButton, 3, 0, 1, 1, Qt::AlignCenter);
    layout->setColumnStretch(0, 2);
    layout->setColumnMinimumWidth(0, 400);
    connect(_closeButton, SIGNAL(clicked()), this, SLOT(close()));
}
