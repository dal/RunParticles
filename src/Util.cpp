#include "Util.h"

#include <time.h>

unsigned int 
Util::parseTime(const char* timeCStr) 
{
    struct tm thisTime;
    strptime(timeCStr, "%Y-%m-%dT%H:%M:%SZ", &thisTime);
    return static_cast<uint>(timegm(&thisTime));
};

QString
Util::secondsToString(int seconds)
{
    int hours = seconds / 3600;
    int modhrs = int(seconds) % 3600;
    int mins = modhrs / 60;
    int secs = modhrs % 60;
    QChar zero('0');
    if (hours > 1)
        return QString("%1hr%2m%3s").arg(hours)
                                    .arg(mins, 2, 10, zero)
                                    .arg(secs, 2, 10, zero);
    else
        return QString("%1m%2s").arg(mins, 2, 10, zero)
                                .arg(secs, 2, 10, zero);
}

