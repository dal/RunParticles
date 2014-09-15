//
//  FitFileReader.h
//  RunParticles
//
//  Created by Doug Letterman on 9/10/14.
//
//

#ifndef __RunParticles__FitFileReader__
#define __RunParticles__FitFileReader__

#include <QString>

#include "fitsdk/fit_decode.hpp"
#include "fitsdk/fit_file_id_mesg.hpp"
#include "fitsdk/fit_file_id_mesg_listener.hpp"
#include "fitsdk/fit_mesg_broadcaster.hpp"
#include "fitsdk/fit_activity_mesg.hpp"
#include "fitsdk/fit_activity_mesg_listener.hpp"
#include "fitsdk/fit_record_mesg.hpp"
#include "fitsdk/fit_record_mesg_listener.hpp"
#include "fitsdk/fit_lap_mesg.hpp"
#include "fitsdk/fit_lap_mesg_listener.hpp"

#include <iostream>

#include "Types.h"

class FitListener : public fit::FileIdMesgListener,
                    public fit::ActivityMesgListener,
                    public fit::RecordMesgListener,
                    public fit::LapMesgListener,
                    public fit::MesgListener
{

public:
    
    FitListener(QList<Track*> *Tracks);
    
    void OnMesg(fit::FileIdMesg& mesg);
    
    void OnMesg(fit::ActivityMesg& mesg);
    
    void OnMesg(fit::LapMesg& mesg);
    
    void OnMesg(fit::RecordMesg& mesg);
    
    void OnMesg(fit::Mesg& mesg);
    
    bool success;
    
    const char* error;
    
protected:
    QList<Track*> *_tracks;
    
    Track *_currentTrack;
    
};

class FitFileReader
{
public:
    
    static bool IsFitFile(const QString &path);
    
    FitFileReader(QList<Track*>*);
    
    ~FitFileReader() { };
    
    bool readFile(const QString &path, char **whyNot=NULL);
    
protected:
    QList<Track*> *_tracks;
    
};


#endif /* defined(__RunParticles__FitFileReader__) */
