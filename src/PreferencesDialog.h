//
//  PreferencesDialog.h
//  RunParticles
//
//  Created by Doug Letterman on 11/10/14.
//
//

#ifndef __RunParticles__PreferencesDialog__
#define __RunParticles__PreferencesDialog__

#include <QAction>
#include <QColorDialog>
#include <QDialog>
#include <QPushButton>

#include "Settings.h"
#include "TableWidgetDragRows.h"

class TrackStyleTableWidget;

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    
    PreferencesDialog(QWidget *parent=NULL);
    
    virtual ~PreferencesDialog() { };
    
    void loadSettings(Settings *settings);
    
    TrackStyleRules getTrackStyleRules() const;
    
public slots:
    
    void slotCellDoubleClicked(int row, int col);
    
    void slotAddRuleButtonClicked();
    
    void slotRemoveRule();
    
protected:
    enum {
        TrackStyleTable_Pattern_Col,
        TrackStyleTable_Color_Col,
        TrackStyleTable_Width_Col,
        TrackStyleTable_Num_Cols
    };
    
    void _addRuleRow(const QString &pattern, const QColor &color, int width);
    
    TableWidgetDragRows *_tableWidget;
    QAction *_removeRuleAction;
    QPushButton *_addRuleButton, *_saveButton, *_cancelButton;
    QColorDialog *_colorDialog;

};

#endif /* defined(__RunParticles__PreferencesDialog__) */
