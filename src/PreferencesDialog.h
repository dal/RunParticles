//
//  PreferencesDialog.h
//  RunParticles
//
//  Created by Doug Letterman on 11/10/14.
//
//

#ifndef __RunParticles__PreferencesDialog__
#define __RunParticles__PreferencesDialog__

#include <QDialog>
#include <QPushButton>
#include <QTableWidget>

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    
    PreferencesDialog(QWidget *parent=NULL);
    
    virtual ~PreferencesDialog() { };
    
protected:
    QTableWidget *_tableWidget;
    QPushButton *_saveButton, *_cancelButton;

};

#endif /* defined(__RunParticles__PreferencesDialog__) */
