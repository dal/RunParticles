//
//  Settings.h
//  RunParticles
//
//  Created by Doug Letterman on 11/1/14.
//
//

#ifndef __RunParticles__Settings__
#define __RunParticles__Settings__

#include <QObject>

#include <QList>
#include <QSettings>
#include <QString>
#include <QWidget>

struct TrackColorPref
{
    QString pattern;
    QColor color;
    
    QVariant toVariant() const;
    static TrackColorPref fromVariant(const QVariant &var);
};

struct TrackColorPrefs
{
    QList<TrackColorPref> prefs;
    
    QVariant toVariant() const;
    
    QColor getColorForTrackType(const QString &type);
    
    static TrackColorPrefs fromVariant(const QVariant &var);
    
    static TrackColorPrefs getDefaultPrefs();
};

class Settings : public QObject
{
    Q_OBJECT
    
public:
    
    Settings(QObject *parent=NULL);
    
    virtual ~Settings() { };
    
    bool restoreWidgetState(QWidget *widget);
    
    void saveWidgetState(const QWidget *widget);
    
    QStringList getRecentMapFiles();
    
    void setRecentMapFiles(const QList<QString> &files);
    
    QStringList getRecentLayerFiles();
    
    void setRecentLayerFiles(const QList<QString> &files);
    
    TrackColorPrefs getTrackColorPrefs();
    
    void setTrackColorPrefs(const TrackColorPrefs &prefs);
    
protected:
    
    QStringList _getList(const QString &key);
    
    void _setList(const QString &key, const QList<QString> &files);
    
    QSettings *_settings;
    
};

#endif /* defined(__RunParticles__Settings__) */
