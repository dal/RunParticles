//
//  Settings.cpp
//  RunParticles
//
//  Created by Doug Letterman on 11/1/14.
//
//

#include "Settings.h"

#include <fnmatch.h>

QVariant
TrackColorPref::toVariant() const
{
    QList<QVariant> var;
    var.append(QVariant(pattern));
    var.append(QVariant(color.rgba()));
    return var;
}

TrackColorPref
TrackColorPref::fromVariant(const QVariant &var)
{
    TrackColorPref pref;
    QList<QVariant> parts = var.toList();
    pref.pattern = parts[0].toString();
    pref.color = QColor::fromRgba(parts[1].toUInt());
    return pref;
}

QVariant
TrackColorPrefs::toVariant() const
{
    QList<QVariant> vars;
    TrackColorPref myPref;
    foreach(myPref, prefs) {
        vars.append(myPref.toVariant());
    }
    return QVariant(vars);
}

QColor
TrackColorPrefs::getColorForTrackType(const QString &type)
{
    TrackColorPref myPref;
    const char* theType = qPrintable(type);
    foreach(myPref, prefs) {
        const char* myPattern = qPrintable(myPref.pattern);
        if (fnmatch(myPattern, theType, FNM_PATHNAME) == 0)
            return myPref.color;
    }
    return QColor(0, 0, 0, 0);
}

TrackColorPrefs
TrackColorPrefs::fromVariant(const QVariant &var)
{
    TrackColorPrefs myPrefs;
    QList<QVariant> vars = var.toList();
    QVariant myVar;
    foreach(myVar, vars)
    {
        myPrefs.prefs.append(TrackColorPref::fromVariant(myVar));
    }
    return myPrefs;
}

TrackColorPrefs
TrackColorPrefs::getDefaultPrefs()
{
    TrackColorPrefs myPrefs;
    TrackColorPref runPref;
    runPref.pattern = QString("run");
    runPref.color = QColor(255, 0, 0);
    TrackColorPref otherPrefs;
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

TrackColorPrefs
Settings::getTrackColorPrefs()
{
    if (_settings->contains("trackColors"))
        return TrackColorPrefs::fromVariant(_settings->value("trackColors"));
    return TrackColorPrefs::getDefaultPrefs();
}

void
Settings::setTrackColorPrefs(const TrackColorPrefs &prefs)
{
    _settings->setValue("trackColors", prefs.toVariant());
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

