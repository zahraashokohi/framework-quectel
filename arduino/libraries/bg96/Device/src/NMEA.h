/*
 * NMEA.h
 *
 *  Created on: 26.06.2018
 *      Author: georgi.angelov
 */

#ifndef _NMEA_H_
#define _NMEA_H_

#include <interface.h>
#include <qapi_quectel.h>
#include <minmea.h>
// /https://github.com/kosma/minmea

class NMEA
{
public:

    static NMEA &getInstance()
    {
        static NMEA myInstance;
        return myInstance;
    }

    int begin(qapi_QT_Loc_CB_t callback)
    {
        if (!callback)
            return -1;
        return qapi_QT_Loc_Start(QT_LOC_EVENT_MASK_NMEA, callback);
    }

    void end()
    {
        qapi_QT_Loc_Stop();
    }

};

#endif /* _NMEA_H_ */
