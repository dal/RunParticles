#include "OsmTileSource.h"

#include <QtGlobal>

#include "Singleton.h"

#include <cinder/Buffer.h>
#include <cinder/DataSource.h>
#include <cinder/imageIO.h>

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
    return *_memoryTileCache[idx].surface;
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
        qDebug("Network Error: %s", reply->errorString().toAscii().constData());
        return;
    }
    
    QByteArray bytes = reply->readAll();
    
    cinder::Buffer buffer((void*)bytes.constData(), bytes.size() + 1);
    cinder::DataSourceBufferRef data = cinder::DataSourceBuffer::create(buffer);
    cinder::Surface8u *surf = new cinder::Surface8u(cinder::loadImage(data,
                                                    cinder::ImageSource::Options(),
                                                    "png"));
    _memoryTileCache.insert(
        std::pair<OsmIndex, OsmTile>(index, OsmTile(index, surf)));
    
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
    
    QNetworkAccessManager *network =
        &Singleton<QNetworkAccessManager>::Instance();
    qDebug("GET %s%s", host.toAscii().constData(), url.toAscii().constData());
    QNetworkReply *reply = network->get(request);
    _pendingReplies.insert(reply, index);
    QObject::connect(reply,
                     SIGNAL(finished()),
                     this,
                     SLOT(onRequestFinished()));
}