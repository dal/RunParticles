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

void
FitListener::OnMesg(fit::Mesg &mesg)
{
    
}

void
FitListener::OnMesg(fit::FileIdMesg& mesg)
{
    
}

void
FitListener::OnMesg(fit::ActivityMesg& mesg)
{
    mesg.GetType();
}

void
FitListener::OnMesg(fit::LapMesg& mesg)
{
    
}

void
FitListener::OnMesg(fit::RecordMesg& mesg)
{
    
}

FitReader::FitReader()
{
    
}

void
ReadFile(const QString &path)
{
    std::fstream file;
    
}
