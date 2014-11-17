//
//  Settings.cpp
//  RunParticles
//
//  Created by Doug Letterman on 11/1/14.
//
//

#include "Settings.h"

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
    const char* theType = qPrintable(type);
    foreach(myPref, prefs) {
        const char* myPattern = qPrintable(myPref.pattern);
        if (fnmatch(myPattern, theType, FNM_PATHNAME) == 0)
            return myPref;
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
TrackStyleRules::getDefaultPrefs()
{
    TrackStyleRules myPrefs;
    TrackStyleRule runPref;
    runPref.pattern = QString("Running");
    runPref.color = QColor(255, 0, 0);
    TrackStyleRule otherPrefs;
    otherPrefs.pattern = QString("*");
    otherPrefs.color = QColor(0, 0, 255);
    myPrefs.prefs << runPref << otherPrefs;
    return myPrefs;
}

Settings::Settings(QObject *parent) :
    QObject(parent),
    _settings(new QSettings(this))
{
    
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
    _settings->setValue("size", widget->size());
    _settings->setValue("pos", widget->pos());
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
    return TrackStyleRules::getDefaultPrefs();
}

void
Settings::setTrackStyleRules(const TrackStyleRules &rules)
{
    _settings->setValue("trackStyleRules", rules.toVariant());
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

