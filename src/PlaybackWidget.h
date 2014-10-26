//
//  PlaybackWidget.h
//  RunParticles
//
//  Created by Doug Letterman on 10/26/14.
//
//

#ifndef __RunParticles__PlaybackWidget__
#define __RunParticles__PlaybackWidget__

#include <QComboBox>
#include <QLineEdit>
#include <QWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QStatusBar>

class PlaybackWidget : public QWidget
{
    Q_OBJECT
    
public:
    PlaybackWidget(QWidget *parent=NULL);
    
    virtual ~PlaybackWidget() { };
    
public slots:
    void setTime(double);
    void setTimeSliderMaximum(int);
    void updateStatus(const QString &message, int total, int done);
    void hideProgress();
    
signals:
    void signalRewind();
    void signalBack();
    void signalPause();
    void signalForward();
    void signalPlaybackRateChanged(const QString&);
    void signalTimeSliderChanged(int);
    
protected:
    void _layoutPlaybackControls();
    
    QPushButton *_rewindButton, *_forwardButton, *_pauseButton, *_backButton;
    QSlider *_slider;
    QLineEdit *_currentTimeLineEdit;
    QComboBox *_playSpeedCombo;
    QStatusBar *_statusBar;
    QProgressBar *_progressBar;

};

#endif /* defined(__RunParticles__PlaybackWidget__) */
