//
//  PreferencesDialog.h
//  RunParticles
//
//  Created by Doug Letterman on 11/10/14.
//
//

#ifndef __RunParticles__PreferencesDialog__
#define __RunParticles__PreferencesDialog__

#include <QColorDialog>
#include <QDialog>
#include <QPushButton>
#include <QTableWidget>

#include "Settings.h"

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    
    PreferencesDialog(QWidget *parent=NULL);
    
    virtual ~PreferencesDialog() { };
    
    void loadSettings(Settings *settings);
    
public slots:
    
    void slotCellDoubleClicked(int row, int col);
    
protected:
    enum {
        TrackColorTable_Pattern_Col,
        TrackColorTable_Color_Col,
        TrackColorTable_Num_Cols
    };
    
    QTableWidget *_tableWidget;
    QPushButton *_saveButton, *_cancelButton;
    QColorDialog *_colorDialog;

};

#endif /* defined(__RunParticles__PreferencesDialog__) */
