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

#include "BoundingBox.h"
#include "Types.h"

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
    
    static TrackStyleRules getDefaultRules();
};

class Settings : public QObject
{
    Q_OBJECT
    
public:
    
    Settings(QObject *parent=NULL);
    
    virtual ~Settings() { };
    
    void clear();
    
    bool restoreWidgetState(QWidget *widget);
    
    void saveWidgetState(const QWidget *widget);
    
    QStringList getRecentMapFiles();
    
    void setRecentMapFiles(const QList<QString> &files);
    
    QStringList getRecentLayerFiles();
    
    void setRecentLayerFiles(const QList<QString> &files);
    
    TrackStyleRules getTrackStyleRules();
    
    void setTrackStyleRules(const TrackStyleRules &rules);
    
    LonLatBox getStartingViewArea();
    
    void setStartingViewArea(const LonLatBox &viewArea);
    
    bool getShowOpenStreetMap();
    
    void setShowOpenStreetMap(bool show);
    
    bool getFrameLastAddedLayer();
    
    void setFrameLastAddedLayer(bool frame);
    
    bool getSaveRelativePaths();
    
    void setSaveRelativePaths(bool rel);
    
protected:
    
    QStringList _getList(const QString &key);
    
    void _setList(const QString &key, const QList<QString> &files);
    
    QSettings *_settings;
    
};

#endif /* defined(__RunParticles__Settings__) */
