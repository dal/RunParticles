//
//  Settings.cpp
//  RunParticles
//
//  Created by Doug Letterman on 11/1/14.
//
//

#include "Settings.h"

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

