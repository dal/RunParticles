#ifndef XMLHANDLER_H
#define XMLHANDLER_H

#include <QXmlDefaultHandler>
#include <QXmlAttributes>

#include "Types.h"

using namespace std;

enum TrackFileType {
    TrackFileType_Unknown,
    TrackFileType_Gpx,
    TrackFileType_Tcx,
    TrackFileType_RunParticlesMap
};

class XmlHandler : public QXmlDefaultHandler
{

public:
    
    bool startElement(const QString&, 
                      const QString&, 
                      const QString&, 
                      const QXmlAttributes&);
    
    TrackFileType getType() const { return _trackFileType; }
    
protected:
    TrackFileType _trackFileType;

};

#endif
