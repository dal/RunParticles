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

FitListener::FitListener(QList<Track*> *tracks) :
    success(true),
    _tracks(tracks)
{
    
}

void
FitListener::OnMesg(fit::FileIdMesg& mesg)
{
    
}

void
FitListener::OnMesg(fit::ActivityMesg& mesg)
{
    _currentTrack = new Track();
    _tracks->append(_currentTrack);
    FIT_ACTIVITY_TYPE type = mesg.GetType();
    if (type == FIT_ACTIVITY_TYPE_RUNNING)
        _currentTrack->sport = "Running";
    else if (type == FIT_ACTIVITY_TYPE_CYCLING)
        _currentTrack->sport = "Cycling";
    else if (type == FIT_ACTIVITY_TYPE_SWIMMING)
        _currentTrack->sport = "Swimming";
    else if (type == FIT_ACTIVITY_TYPE_TRANSITION)
        _currentTrack->sport = "Transition";
    else if (type == FIT_ACTIVITY_TYPE_WALKING)
        _currentTrack->sport = "Walking";
    else if (type == FIT_ACTIVITY_TYPE_GENERIC)
        _currentTrack->sport = "Generic";
    else
        _currentTrack->sport = "Unknown";
    
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
    // seconds since UTC 00:00 Dec 31 1989
    FIT_DATE_TIME timestamp = mesg.GetTimestamp();
    FIT_SINT32 lat = mesg.GetPositionLat();
    FIT_SINT32 lng = mesg.GetPositionLong();
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
    broadcaster.AddListener((fit::ActivityMesgListener &)listener);
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
