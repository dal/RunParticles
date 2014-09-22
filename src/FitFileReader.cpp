//
//  FitFileReader.cpp
//  RunParticles
//
//  Created by Doug Letterman on 9/10/14.
//
//

#include "FitFileReader.h"

#include <QString>
#include <fstream>

#define SEMICIRCLE_TO_DMS 8.381903171539307e-08

// fit timestamps are expressed in seconds since UTC 00:00 Dec 31 1989
// or epoch (631094400)
#define FIT_EPOCH_OFFSET 631094400

FitListener::FitListener(QList<Track*> *tracks) :
    success(true),
    _tracks(tracks),
    _currentTrack(new Track())
{
    
}

void
FitListener::OnMesg(fit::FileIdMesg& mesg)
{
    
}

void
FitListener::OnMesg(fit::SessionMesg& mesg)
{
    FIT_SPORT sport = mesg.GetSport();
    if (sport == FIT_SPORT_RUNNING)
        _currentTrack->sport = "Running";
    else if (sport == FIT_SPORT_CYCLING)
        _currentTrack->sport = "Cycling";
    else if (sport == FIT_SPORT_SWIMMING)
        _currentTrack->sport = "Swimming";
    else if (sport == FIT_SPORT_TRANSITION)
        _currentTrack->sport = "Transition";
    else if (sport == FIT_SPORT_FITNESS_EQUIPMENT)
        _currentTrack->sport = "Fitness equipment";
    else if (sport == FIT_SPORT_WALKING)
        _currentTrack->sport = "Walking";
    else if (sport == FIT_SPORT_GENERIC)
        _currentTrack->sport = "Generic";
    else if (sport == FIT_SPORT_WALKING)
        _currentTrack->sport = "Walking";
    else if (sport == FIT_SPORT_CROSS_COUNTRY_SKIING)
        _currentTrack->sport = "Cross country skiing";
    else if (sport == FIT_SPORT_ALPINE_SKIING)
        _currentTrack->sport = "Alpine skiing";
    else if (sport == FIT_SPORT_SNOWBOARDING)
        _currentTrack->sport = "Snowboarding";
    else if (sport == FIT_SPORT_ROWING)
        _currentTrack->sport = "Rowing";
    else if (sport == FIT_SPORT_MOUNTAINEERING)
        _currentTrack->sport = "Mountaineering";
    else if (sport == FIT_SPORT_HIKING)
        _currentTrack->sport = "Hiking";
    else if (sport == FIT_SPORT_MULTISPORT)
        _currentTrack->sport = "Multisport";
    else if (sport == FIT_SPORT_PADDLING)
        _currentTrack->sport = "Paddling";
    else if (sport == FIT_SPORT_ALL)
        _currentTrack->sport = "All";
    else
        _currentTrack->sport = "Unknown";
    
    // Set the name from the timestamp
    FIT_DATE_TIME tstmp = mesg.GetStartTime() + FIT_EPOCH_OFFSET;
    _currentTrack->name = QDateTime::fromTime_t(tstmp)
                          .toString("yyyy-MM-ddThh:mm:ssZ");
    
    _tracks->append(_currentTrack);
    _currentTrack = new Track();
}

void
FitListener::OnMesg(fit::LapMesg& mesg)
{
    
}

void
FitListener::OnMesg(fit::RecordMesg& mesg)
{
    if (!_currentTrack)
        return;
    FIT_DATE_TIME timestamp = mesg.GetTimestamp() + FIT_EPOCH_OFFSET;
    FIT_SINT32 lat = mesg.GetPositionLat();
    FIT_SINT32 lng = mesg.GetPositionLong();
    TrackPoint point;
    point.pos.y = (double)lat * SEMICIRCLE_TO_DMS;
    point.pos.x = (double)lng * SEMICIRCLE_TO_DMS;
    if (point.pos.x > 180.)
        point.pos.x -= 360.;
    if (point.pos.y > 180.)
        point.pos.y -= 360;
    if (point.pos.x > 180. || point.pos.x < -180.
        || point.pos.y > 90. || point.pos.y < -90.)
        return;
    point.time = timestamp;
    _currentTrack->points.push_back(point);
}

void
FitListener::OnMesg(fit::Mesg &mesg)
{
    
}

bool
FitFileReader::IsFitFile(const QString &path)
{
    bool isit = false;
    std::fstream file;
    file.open(path.toLocal8Bit().constData(),
              std::ios::in | std::ios::binary);
    if (file.is_open()) {
        fit::Decode decode;
        isit = decode.IsFIT(file);
    }
    return isit;
}

FitFileReader::FitFileReader(QList<Track*>*tracks) :
    _tracks(tracks)
{
    
}

bool
FitFileReader::readFile(const QString &path, char **whyNot)
{
    std::fstream file;
    fit::Decode decode;
    fit::MesgBroadcaster broadcaster;
    FitListener listener(_tracks);
    
    file.open(path.toLocal8Bit().constData(),
              std::ios::in | std::ios::binary);
    
    if (!file.is_open())
    {
        if (whyNot)
            sprintf(*whyNot, "Error opening file");
        return false;
    }
    
    if (!decode.CheckIntegrity(file))
    {
        if (whyNot)
            sprintf(*whyNot, "FIT file integrity failed");
        return false;
    }
    
    broadcaster.AddListener((fit::FileIdMesgListener &)listener);
    broadcaster.AddListener((fit::SessionMesgListener &)listener);
    broadcaster.AddListener((fit::LapMesgListener &)listener);
    broadcaster.AddListener((fit::RecordMesgListener &)listener);
    
    try
    {
        broadcaster.Run(file);
    }
    catch (const fit::RuntimeException& e)
    {
        sprintf(*whyNot, "Exception decoding file: %s\n", e.what());
        return false;
    }
    
    return true;
}
