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
    
    _textEdit = new QTextEdit(QString("<h2>RunParticles v%0</h2>"
                                      "<p>&copy;2014 by Doug Letterman<br />"
                                      "<a href=\"http://www.renderfast.com/\">"
                                      "renderfast.com</a></p>"
                                      "<p>Oakland, California, USA</p>"
                                      "<p><em>RunParticles is free software "
                                      "distributed under the terms of the "
                                      "MIT&nbsp;License</em></p>")
                                      .arg(qApp->applicationVersion()),
                              this);
    _textEdit->setFrameStyle(QFrame::NoFrame);
    _textEdit->setReadOnly(true);
    _textEdit->setAlignment(Qt::AlignCenter);
    _textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QPalette p;
    p.setColor(QPalette::Base, p.color(QPalette::Background));
    _textEdit->setPalette(p);
    
    _closeButton = new QPushButton("Close", this);
    
    /* layout */
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(_icon, 0, 0, 1, 1, Qt::AlignCenter);
    layout->addWidget(_textEdit, 1, 0);
    layout->addWidget(_closeButton, 2, 0, 1, 1, Qt::AlignCenter);
    layout->setColumnStretch(0, 2);
    layout->setColumnMinimumWidth(0, 400);
    connect(_closeButton, SIGNAL(clicked()), this, SLOT(close()));
}
