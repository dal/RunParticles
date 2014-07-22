#include "OsmTileSource.h"

#include "Singleton.h"

void
OsmTileSource::getTile(int x, int y, int z)
{
    OsmIndex index = OsmIndex(z, x, y);
    if (_memoryTileCache.find(index) != _memoryTileCache.end()) {
        emit tileReady(x, y, z);
        return;
    }
    
    _requestTile(index);
}

void
OsmTileSource::onRequestFinished(QNetworkReply *reply)
{
    
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
    
    QNetworkAccessManager *network =
        &Singleton<QNetworkAccessManager>::Instance();
    QNetworkReply *reply = network->get(request);
    _pendingReplies.insert(reply, index);
    QObject::connect(reply,
                     SIGNAL(finished()),
                     this,
                     SLOT(onRequestFinished()));
}