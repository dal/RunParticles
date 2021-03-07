//
//  Settings.cpp
//  RunParticles
//
//  Created by Doug Letterman on 11/1/14.
//
//

#include "Settings.h"

#include <QApplication>
#include <QDesktopWidget>

#include <fnmatch.h>

TrackStyleRule::TrackStyleRule() :
    pattern("*"),
    color(QColor(255,0,0)),
    width(1.0)
{
    
}

QVariant
TrackStyleRule::toVariant() const
{
    QList<QVariant> var;
    var.append(QVariant(pattern));
    var.append(QVariant(color.rgba()));
    var.append(QVariant(width));
    return var;
}

TrackStyleRule
TrackStyleRule::fromVariant(const QVariant &var)
{
    TrackStyleRule pref;
    QList<QVariant> parts = var.toList();
    int size = parts.size();
    if (size >= 1) {
        pref.pattern = parts[0].toString();
        if (size >= 2) {
            pref.color = QColor::fromRgba(parts[1].toUInt());
            if (size >= 3)
                pref.width = parts[2].toUInt();
        }
    }
    return pref;
}

QVariant
TrackStyleRules::toVariant() const
{
    QList<QVariant> vars;
    TrackStyleRule myPref;
    foreach(myPref, prefs) {
        vars.append(myPref.toVariant());
    }
    return QVariant(vars);
}

TrackStyleRule
TrackStyleRules::getStyleForTrackType(const QString &type) const
{
    TrackStyleRule myPref;
    QByteArray theType = type.toLocal8Bit();
    foreach(myPref, prefs) {
        QByteArray myPattern = myPref.pattern.toLocal8Bit();
        if (fnmatch(myPattern.data(), theType.data(), FNM_PATHNAME) == 0) {
            return myPref;
        }
    }
    return TrackStyleRule();
}

TrackStyleRules
TrackStyleRules::fromVariant(const QVariant &var)
{
    TrackStyleRules myPrefs;
    QList<QVariant> vars = var.toList();
    QVariant myVar;
    foreach(myVar, vars)
    {
        myPrefs.prefs.append(TrackStyleRule::fromVariant(myVar));
    }
    return myPrefs;
}

TrackStyleRules
TrackStyleRules::getDefaultRules()
{
    TrackStyleRules myPrefs;
    TrackStyleRule runPref, bikePref, cyclingPref, otherPrefs;
    runPref.pattern = QString("Running");
    runPref.color = QColor(255, 0, 0);
    bikePref.pattern = QString("Biking");
    bikePref.color = QColor(0, 0, 255);
    cyclingPref.pattern = QString("Cycling");
    cyclingPref.color = QColor(0, 0, 255);
    otherPrefs.pattern = QString("*");
    otherPrefs.color = QColor(0, 255, 0);
    myPrefs.prefs << runPref << bikePref << cyclingPref << otherPrefs;
    return myPrefs;
}

Settings::Settings(QObject *parent) :
    QObject(parent),
    _settings(new QSettings(this))
{
    
}

void
Settings::clear()
{
    _settings->clear();
}

bool
Settings::restoreWidgetState(QWidget *widget)
{
    _settings->beginGroup(QString("widgetGeometry/%0").arg(widget->objectName()));
    bool hasState;
    if ((hasState = _settings->contains("size")))
        widget->resize(_settings->value("size").toSize());
    if (_settings->contains("pos"))
        widget->move(_settings->value("pos").toPoint());
    _settings->endGroup();
    return hasState;
}

void
Settings::saveWidgetState(const QWidget *widget)
{
    _settings->beginGroup(QString("widgetGeometry/%0").arg(widget->objectName()));
    QPoint pos = widget->pos();
    _settings->setValue("size", widget->size());
    _settings->setValue("pos", pos);
    _settings->endGroup();
}

QStringList
Settings::getRecentMapFiles()
{
    return _getList("recentMapFiles");
}

void
Settings::setRecentMapFiles(const QList<QString> &files)
{
    _setList("recentMapFiles", files);
}

QStringList
Settings::getRecentLayerFiles()
{
    return _getList("recentLayerFiles");
}

void
Settings::setRecentLayerFiles(const QList<QString> &files)
{
    _setList("recentLayerFiles", files);
}

TrackStyleRules
Settings::getTrackStyleRules()
{
    if (_settings->contains("trackStyleRules"))
        return TrackStyleRules::fromVariant(_settings->value("trackStyleRules"));
    return TrackStyleRules::getDefaultRules();
}

void
Settings::setTrackStyleRules(const TrackStyleRules &rules)
{
    _settings->setValue("trackStyleRules", rules.toVariant());
}

LonLatBox
Settings::getStartingViewArea()
{
    if (_settings->contains("startingViewArea")) {
        QVariantList vars = _settings->value("startingViewArea").toList();
        if (vars.length() == 4) {
            double bounds[4];
            bool ok = true;
            for (int i=0; i<4 && ok; i++) {
                bounds[i] = vars[i].toDouble(&ok);
            }
            if (ok)
                return LonLatBox(bounds[0], bounds[1], bounds[2], bounds[3]);
        }
    }
    // Default is centered over Oakland
    return LonLatBox(-122.37, 37.93, -122.27, 37.73);
}

void
Settings::setStartingViewArea(const LonLatBox &viewArea)
{
    QVariantList bounds;
    bounds << QVariant(viewArea.upperLeft.x)
           << QVariant(viewArea.upperLeft.y)
           << QVariant(viewArea.lowerRight.x)
           << QVariant(viewArea.lowerRight.y);
    _settings->setValue("startingViewArea", bounds);
}

bool
Settings::getShowOpenStreetMap()
{
    return _settings->value("showOpenStreetMap", QVariant(true)).toBool();
}

void
Settings::setShowOpenStreetMap(bool show)
{
    _settings->setValue("showOpenStreetMap", show);
}

bool
Settings::getFrameLastAddedLayer()
{
    return _settings->value("frameLastAddedLayer", QVariant(true)).toBool();
}

void
Settings::setFrameLastAddedLayer(bool frame)
{
    _settings->setValue("frameLastAddedLayer", frame);
}

bool
Settings::getSaveRelativePaths()
{
    return _settings->value("saveRelativePaths", QVariant(false)).toBool();
}

void
Settings::setSaveRelativePaths(bool rel)
{
    _settings->setValue("saveRelativePaths", rel);
}

QStringList
Settings::_getList(const QString &key)
{
    QList<QString> files;
    QList<QVariant> strVars = _settings->value(key).toList();
    QVariant myvar;
    foreach(myvar, strVars) {
        files.append(myvar.toString());
    }
    return files;
}

void
Settings::_setList(const QString &key, const QList<QString> &files)
{
    QList<QVariant> varList;
    QString myFile;
    foreach(myFile, files) {
        varList.append(QVariant(myFile));
    }
    _settings->setValue(key, QVariant(varList));
}

