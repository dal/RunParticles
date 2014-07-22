#include "OsmTileSource.h"

#include <QtGlobal>

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