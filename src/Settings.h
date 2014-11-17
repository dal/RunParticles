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

struct TrackStyleRule
{
    TrackStyleRule();
    QString pattern;
    QColor color;
    unsigned int width;
    
    QVariant toVariant() const;
    static TrackStyleRule fromVariant(const QVariant &var);
};

struct TrackStyleRules
{
    QList<TrackStyleRule> prefs;
    
    QVariant toVariant() const;
    
    TrackStyleRule getStyleForTrackType(const QString &type) const;
    
    static TrackStyleRules fromVariant(const QVariant &var);
    
    static TrackStyleRules getDefaultPrefs();
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
    
    TrackStyleRules getTrackStyleRules();
    
    void setTrackStyleRules(const TrackStyleRules &rules);
    
protected:
    
    QStringList _getList(const QString &key);
    
    void _setList(const QString &key, const QList<QString> &files);
    
    QSettings *_settings;
    
};

#endif /* defined(__RunParticles__Settings__) */
