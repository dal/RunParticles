//
//  PlaybackWidget.cpp
//  RunParticles
//
//  Created by Doug Letterman on 10/26/14.
//
//

#include "PlaybackWidget.h"

#include <QGridLayout>
#include <QVBoxLayout>

#include "Util.h"

PlaybackWidget::PlaybackWidget(QWidget *parent) :
    QWidget(parent)
{
    setObjectName("PlaybackWidget");
    setWindowTitle("Playback controls");
    _rewindButton = new QPushButton(QString::fromUtf8("\u25C0\u25C0"), this);//<<
    _backButton = new QPushButton(QString::fromUtf8("\u25C0"), this); // <
    _pauseButton = new QPushButton(QString::fromUtf8("\u2588"), this); // [ ]
    _forwardButton = new QPushButton(QString::fromUtf8("\u25BA"), this); // >
    _playSpeedCombo = new QComboBox(this);
    _playSpeedCombo->addItem("1x");
    _playSpeedCombo->addItem("2x");
    _playSpeedCombo->addItem("4x");
    _playSpeedCombo->addItem("8x");
    _playSpeedCombo->addItem("16x");
    _playSpeedCombo->addItem("32x");
    _playSpeedCombo->addItem("64x");
    _playSpeedCombo->addItem("128x");
    _playSpeedCombo->addItem("256x");
    _slider = new QSlider(Qt::Horizontal, this);
    _currentTimeLineEdit = new QLineEdit(this);
    _currentTimeLineEdit->setReadOnly(true);
    _currentTimeLineEdit->setText("0");
    _currentTimeLineEdit->setAlignment(Qt::AlignRight);
    _statusBar = new QStatusBar(this);
    _progressBar = new QProgressBar();
    _statusBar->addPermanentWidget(_progressBar);
    _statusBar->show();
    _layoutPlaybackControls();
    
    /* connect the buttons up to relay signals */
    connect(_rewindButton, SIGNAL(clicked()), SIGNAL(signalRewind()));
    connect(_backButton, SIGNAL(clicked()), SIGNAL(signalBack()));
    connect(_pauseButton, SIGNAL(clicked()), SIGNAL(signalPause()));
    connect(_forwardButton, SIGNAL(clicked()), SIGNAL(signalForward()));
    connect(_playSpeedCombo, SIGNAL(activated(const QString&)),
            SIGNAL(signalPlaybackRateChanged(const QString&)));
    connect(_slider, SIGNAL(valueChanged(int)),
            SIGNAL(signalTimeSliderChanged(int)));
    setTimeSliderMaximum(1800);
    hideProgress();
}

void
PlaybackWidget::setTime(double mapSeconds)
{
    QString time = Util::secondsToString(mapSeconds);
    _currentTimeLineEdit->setText(time);
    _slider->blockSignals(true);
    _slider->setSliderPosition(int(mapSeconds));
    _slider->blockSignals(false);
}

void
PlaybackWidget::setTimeSliderMaximum(int max)
{
    _slider->setMaximum(max);
}

void
PlaybackWidget::updateStatus(const QString &message, int total, int done)
{
    if (done < total)
        _progressBar->show();
    _statusBar->showMessage(message);
    _progressBar->setMaximum(total);
    _progressBar->setValue(done);
}

void
PlaybackWidget::hideProgress()
{
    _progressBar->hide();
    _statusBar->clearMessage();
}

void
PlaybackWidget::_layoutPlaybackControls()
{
    QVBoxLayout *mainCol = new QVBoxLayout(this);
    mainCol->setMargin(0);
    mainCol->setSpacing(2);
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(8);
    layout->setSpacing(8);
    layout->addWidget(_rewindButton, 0, 0);
    layout->addWidget(_backButton, 0, 1);
    layout->addWidget(_pauseButton, 0, 2);
    layout->addWidget(_forwardButton, 0, 3);
    layout->addWidget(_playSpeedCombo, 0, 4);
    layout->addWidget(_slider, 1, 0, 1, 4);
    layout->addWidget(_currentTimeLineEdit, 1, 4);
    for (int i=0; i < layout->columnCount()-1; i++)
        layout->setColumnStretch(i, 1);
    layout->setColumnMinimumWidth(4, 100);
    layout->setRowStretch(0, 2);
    /* make the buttons expand vertically */
    _rewindButton->setSizePolicy(QSizePolicy::Preferred,
                                 QSizePolicy::Expanding);
    _backButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    _pauseButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    _forwardButton->setSizePolicy(QSizePolicy::Preferred,
                                  QSizePolicy::Expanding);
    mainCol->addLayout(layout);
    mainCol->addWidget(_statusBar);
}
