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
#include <QCheckBox>
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
    
    PreferencesDialog(Settings *settings, QWidget *parent=NULL);
    
    virtual ~PreferencesDialog() { };
    
    void loadSettings();
    
    void saveSettings();
    
    TrackStyleRules getTrackStyleRules() const;
    
public slots:
    
    void slotCellDoubleClicked(int row, int col);
    
    void slotAddRuleButtonClicked();
    
    void slotRemoveRule();
    
    void slotResetSettings();
    
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
    QPushButton *_addRuleButton, *_resetButton, *_saveButton, *_cancelButton;
    QColorDialog *_colorDialog;
    QCheckBox *_showOpenStreetMapCheckBox, *_frameLastAddedLayerCheckBox,
              *_saveMapRelativePaths;
    
    Settings *_settings;

};

#endif /* defined(__RunParticles__PreferencesDialog__) */
