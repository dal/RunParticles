//
//  ExportImageDialog.h
//  RunParticles
//
//  Created by Doug Letterman on 12/14/14.
//
//

#ifndef __RunParticles__ExportImageDialog__
#define __RunParticles__ExportImageDialog__

#include <QDialog>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QProgressDialog>
#include <QTimer>

#include "GLWidget.h"
#include "PlaybackWidget.h"

class ExportImageDialog : public QDialog
{
    Q_OBJECT
    
public:
    ExportImageDialog(GLWidget *target,
                      PlaybackWidget *playbackWidget,
                      QWidget *parent=0,
                      Qt::WindowFlags f = 0);
    
protected slots:
    void slotExportImage();

protected:
    void _export();
    
    QDoubleSpinBox *_outputFpsSpinBox;
    QSpinBox *_outputFrameCount;
    QPushButton *_exportButton, *_cancelButton;
    GLWidget *_target;
    QLineEdit *_baseImageNameLineEdit;
    PlaybackWidget *_playbackWidget;
    QTimer *_timer;
    QProgressDialog *_progressDialog;
    QString _outputDir;
    int _currentFrame;
    double _startTime;
};

#endif /* defined(__RunParticles__ExportImageDialog__) */
