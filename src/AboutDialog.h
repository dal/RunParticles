//
//  AboutDialog.h
//  RunParticles
//
//  Created by Doug Letterman on 10/28/14.
//
//

#ifndef __RunParticles__AboutDialog__
#define __RunParticles__AboutDialog__

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

class AboutDialog : public QDialog
{
    Q_OBJECT
    
public:
    AboutDialog(QWidget *parent = 0);
    
protected:
    QPushButton *_closeButton;
    QLabel *_icon;
    QTextEdit *_textEdit;
};

#endif /* defined(__RunParticles__AboutDialog__) */
