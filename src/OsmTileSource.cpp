#include "OsmTileSource.h"

void
OsmTileSource::getTile(int x, int y, int z)
{
    
}

void
OsmTileSource::onRequestFinished(QNetworkReply *reply)
{
    
}

void _requestTile(const OsmIndex index) const
{
    QString host = QString("http://b.tile.openstreetmap.org");
    QString url = QString("/%1/%2/%3.png").arg(index.z, index.x, index.y);
    
}