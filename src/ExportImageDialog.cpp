//
//  ExportImageDialog.cpp
//  RunParticles
//
//  Created by Doug Letterman on 12/14/14.
//
//

#include "ExportImageDialog.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QLabel>

ExportImageDialog::ExportImageDialog(GLWidget *target,
                                     PlaybackWidget *playbackWidget,
                                     QWidget *parent,
                                     Qt::WindowFlags f) :
    QDialog(parent, f),
    _target(target),
    _playbackWidget(playbackWidget),
    _timer(new QTimer(this)),
    _currentFrame(0)
{
    setWindowTitle("Export images");
    QLabel *fpsLabel = new QLabel("FPS", this);
    _outputFpsSpinBox = new QDoubleSpinBox(this);
    _outputFpsSpinBox->setMinimum(1.0);
    _outputFpsSpinBox->setValue(30.0);
    QLabel *countLabel = new QLabel("Total number of frames", this);
    _outputFrameCount = new QSpinBox(this);
    _outputFrameCount->setValue(60);
    _outputFrameCount->setMinimum(1);
    _exportButton = new QPushButton("Export images", this);
    _cancelButton = new QPushButton("Cancel", this);
    QLabel *baseNameLabel = new QLabel("Base image name", this);
    _baseImageNameLineEdit = new QLineEdit("RunParticlesExport", this);
    QGridLayout *grid = new QGridLayout(this);
    QHBoxLayout *bottomRow = new QHBoxLayout();
    bottomRow->addStretch();
    bottomRow->addWidget(_cancelButton);
    bottomRow->addWidget(_exportButton);
    grid->addWidget(fpsLabel, 0, 0);
    grid->addWidget(_outputFpsSpinBox, 0, 1);
    grid->addWidget(countLabel, 1, 0);
    grid->addWidget(_outputFrameCount, 1, 1);
    grid->addWidget(baseNameLabel, 2, 0);
    grid->addWidget(_baseImageNameLineEdit, 2, 1);
    grid->addLayout(bottomRow, 3, 0, 1, 3);
    
    _progressDialog = new QProgressDialog("Exporting", "stop", 0, 100, this);
    _timer->setInterval(30);
    
    connect(_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(_exportButton, &QPushButton::clicked,
            this, &ExportImageDialog::_export);
    connect(this, &ExportImageDialog::accepted,
            this, &ExportImageDialog::_export);
    connect(_timer, &QTimer::timeout,
            this, &ExportImageDialog::slotExportImage);
    connect(_progressDialog, &QProgressDialog::canceled, _timer, &QTimer::stop);
}

void ExportImageDialog::_export()
{
    _outputDir = QFileDialog::getExistingDirectory(this);
    _currentFrame = 0;
    _progressDialog->setMaximum(_outputFrameCount->value());
    _progressDialog->show();
    _timer->start();
}

void ExportImageDialog::slotExportImage()
{
    int numFrames = _outputFrameCount->value();
    if (_outputDir.isEmpty()) {
        QMessageBox::critical(this, "Export error", "Output directory not set");
        _timer->stop();
        return;
    }
    
    if (_currentFrame++ < numFrames) {
        int fps = (double)_outputFpsSpinBox->value();
        double playbackRate = _playbackWidget->getPlaybackRate();
        double nowTime = _target->getMapSeconds();
        double newTime = nowTime + (playbackRate / fps);
        _target->setMapSeconds(newTime);
        QString imgName = _baseImageNameLineEdit->text();
        QImage theImage = _target->grabFrameBuffer();
        QString imgPath = QString("%0/%1.%2.tif")
                            .arg(_outputDir)
                            .arg(imgName)
                            .arg(_currentFrame, 4, 10, QChar('0'));
        theImage.save(imgPath);
        _progressDialog->setValue(_currentFrame);
    } else {
        _timer->stop();
        _progressDialog->hide();
        close();
    }
}

