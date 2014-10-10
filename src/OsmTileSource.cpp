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
OsmTileSource::getTile(const OsmIndex &index)
{
    if (_memoryTileCache.find(index) != _memoryTileCache.end()) {
        emit tileReady(index);
        return;
    }
    
    _requestTile(index);
}

bool
OsmTileSource::hasFinishedTile(const OsmIndex &index) const
{
    return _memoryTileCache.find(index) != _memoryTileCache.end();
}

cinder::Surface8u&
OsmTileSource::retrieveFinishedTile(const OsmIndex &index)
{
    return _memoryTileCache[index]->getSurface();
}

void
OsmTileSource::cancelTileRequest(const OsmIndex &index)
{
    OsmIndexSet::const_iterator i = _pendingRequests.find(index);
    if (i == _pendingRequests.end())
        return; // it's not a pending request
    _pendingRequests.erase(i);
    for (OsmReplyMap::Iterator i=_pendingReplies.begin();
         i != _pendingReplies.end(); )
    {
        if (i.value() == index) {
            QNetworkReply *reply = i.key();
            _pendingReplies.remove(reply);
            reply->abort();
            break;
        } else {
            ++i;
        }
    }
    
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
        return; // likely a canceled request
    
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
    // Demo party: Never clean the cache _cleanCache();
    emit tileReady(index);
}

void
OsmTileSource::_requestTile(const OsmIndex index)
{
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
    qDebug() << " OsmIndex(" << index.x << ", " << index.y << ", " << index.z << ");";
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
