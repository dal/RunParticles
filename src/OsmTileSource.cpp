#include "OsmTileSource.h"

#include <QtGlobal>
#include <QtDebug>

#include "Singleton.h"

#include <cinder/Buffer.h>
#include <cinder/DataSource.h>
#include <cinder/imageIO.h>

unsigned int OsmTileSource::OsmTile::_accessCount = 0;

OsmTileSource::OsmTile::OsmTile(OsmIndex idx, cinder::Surface *surf)
: index(idx), _surface(surf), _lastAccess(++_accessCount)
{

};

unsigned int
OsmTileSource::OsmTile::age() const
{
    return _accessCount - _lastAccess;
}

cinder::Surface8u&
OsmTileSource::OsmTile::getSurface()
{
    _lastAccess = ++_accessCount;
    return *_surface;
}

OsmTileSource::OsmTileSource(QObject *parent)
    : QObject(parent),
      _tileCount(0)
{

}

OsmTileSource::~OsmTileSource()
{
    
}

void
OsmTileSource::getTile(unsigned int x, unsigned int y, unsigned int z)
{
    OsmIndex index = OsmIndex(x, y, z);
    if (_memoryTileCache.find(index) != _memoryTileCache.end()) {
        emit tileReady(x, y, z);
        return;
    }
    
    _requestTile(index);
}

cinder::Surface8u&
OsmTileSource::retrieveFinishedTile(unsigned int x,
                                    unsigned int y,
                                    unsigned int z)
{
    OsmIndex idx(x, y, z);
    return _memoryTileCache[idx]->getSurface();
}

void
OsmTileSource::onRequestFinished()
{
    QObject * sender = QObject::sender();
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender);
    
    if (reply == 0)
    {
        qWarning("Failed to cast QNetworkReply");
        return;
    }
    
    reply->deleteLater();
    
    if (!_pendingReplies.contains(reply))
    {
        qWarning("Unknown QNetworkReply");
        return;
    }
    
    OsmIndex index = _pendingReplies.take(reply);
    _pendingRequests.erase(index);

    //If there was a network error, ignore the reply
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning() << "Network Error: " << reply->errorString();
        return;
    }
    
    QByteArray bytes = reply->readAll();
    
    cinder::Buffer buffer((void*)bytes.constData(), bytes.size() + 1);
    cinder::DataSourceBufferRef data = cinder::DataSourceBuffer::create(buffer);
    cinder::Surface8u *surf = new cinder::Surface8u(cinder::loadImage(data,
                                                    cinder::ImageSource::Options(),
                                                    "png"));
    OsmTileRef tileRef = OsmTileRef(new OsmTile(index, surf));
    _memoryTileCache.insert(
        std::pair<OsmIndex, OsmTileRef>(index, tileRef));
    _cleanCache();
    emit tileReady(index.x, index.y, index.z);
}

void
OsmTileSource::_requestTile(const OsmIndex index)
{
    //qDebug("x %d y %d z %d h %d", index.x, index.y, index.z, index.hash());
    if (_pendingRequests.find(index) != _pendingRequests.end())
        return;
    _pendingRequests.insert(index);
    QString host = QString("http://b.tile.openstreetmap.org");
    QString url = QString("/%1/%2/%3.png").arg(QString::number(index.z),
                                               QString::number(index.x),
                                               QString::number(index.y));
    QNetworkRequest request = QNetworkRequest(QUrl(host + url));
    request.setRawHeader("User-Agent", DEFAULT_USER_AGENT);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                 QNetworkRequest::PreferCache);
    QNetworkAccessManager *network =
        Singleton<QNetworkAccessManager>::Instance();
    qDebug() << "GET " << host << url;
    QNetworkReply *reply = network->get(request);
    _pendingReplies.insert(reply, index);
    QObject::connect(reply,
                     SIGNAL(finished()),
                     this,
                     SLOT(onRequestFinished()));
}

void
OsmTileSource::_cleanCache()
{
    if (_memoryTileCache.size() <= memCacheSize * 2)
        return;
    qDebug() << "Cleaning memory tile cache";
    for (OsmTileMap::iterator i=_memoryTileCache.begin();
         i != _memoryTileCache.end();) {
        if (i->second->age() > memCacheSize)
            i = _memoryTileCache.erase(i);
        else
            i++;
    }
}
