/*
 * This file is part of the Oubliette (http://oubliette.sf.net/) import plugin
 * for KeePass (http://keepass.sf.net/).
 *
 * Copyright (C) 2005-2009 Sebastian Schuberth <sschuberth@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef DATETIME_H
#define DATETIME_H

#include <cmath>
#include <ctime>

// This class represents Delphi date and time encoded into a "double".
class DateTime
{
  public:

    DateTime(double date_time=0)
      :m_date_time(date_time),m_need_update(true) {}

    void set(double date_time) {
        m_date_time=date_time;
        m_need_update=true;
    }

    bool isValid() const {
        return m_date_time!=0;
    }

    // Regular year, 2004 means 2004.
    unsigned int getYear() const {
        if (!convertLazyUpdate())
            return 0;
        return m_tm.tm_year+1900;
    }

    // Ranges from 1 to 12.
    unsigned int getMonth() const {
        if (!convertLazyUpdate())
            return 0;
        return m_tm.tm_mon+1;
    }

    // The first day is 1.
    unsigned int getDay() const {
        if (!convertLazyUpdate())
            return 0;
        return m_tm.tm_mday;
    }

    // Ranges from 0 to 23.
    unsigned int getHour() const {
        if (!convertLazyUpdate())
            return 0;
        return m_tm.tm_hour;
    }

    // Ranges from 0 to 59.
    unsigned int getMinute() const {
        if (!convertLazyUpdate())
            return 0;
        return m_tm.tm_min;
    }

    // Ranges from 0 to 59.
    unsigned int getSecond() const {
        if (!convertLazyUpdate())
            return 0;
        return m_tm.tm_sec;
    }

  private:

    static double frac(double x) {
        if (x>0)
            return (x-floor(x));
        if (x<0)
            return (x+floor(-x));
        return 0;
    }

    static double trunc(double x) {
        return x-frac(x);
    }

    static bool convert(double _date_time,tm* _tm) {
        // Offset to 1970-Jan-01 in seconds.
        static time_t const offset=2209161600;

        // Days since 1899-Dec-30.
        time_t days=(time_t)trunc(_date_time);
        // Percentage of the day in seconds.
        time_t percent=(time_t)(fabs(frac(_date_time))*60*60*24);

        time_t seconds=days*60*60*24+percent-offset;
        return _gmtime64_s(_tm,&seconds)==0;
    }

    bool convertLazyUpdate() const {
        if (m_need_update) {
            if (!convert(m_date_time,&m_tm))
                return false;
            m_need_update=false;
        }
        return true;
    }

    double m_date_time;
    mutable tm m_tm;
    mutable bool m_need_update;
};

#endif // DATETIME_H
