/*
  BC66 - rtc
    Created on: 01.01.2019
    Author: Georgi Angelov

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA   
 */

#ifndef __RTC_H__
#define __RTC_H__

#include <Arduino.h>
#include <time.h>

class LRTC : public Printable
{
public:
  LRTC()
  {
    m_year = 0;
    m_month = 0;
    m_day = 0;
    m_hour = 0;
    m_minute = 0;
    m_second = 0;
    m_timezone = 0;
    memset((void *)buf, 0, 32);
  };

  time_t get() { return now(); }
  void get(ST_Time *p)
  {
    if (p)
      Ql_GetLocalTime(p);
  }

  int timezone() { return m_timezone; }
  int year() { return m_year; }
  int month() { return m_month; }
  int day() { return m_day; }
  int hour() { return m_hour; }
  int minute() { return m_minute; }
  int second() { return m_second; }

  const char *ascTime(char *buffer, u32 size) { return s_asctime(NULL); }

  virtual size_t printTo(Print &p) const
  {
    s_asctime(this);
    return p.print(buf);
  }

private:
  int m_year;
  int m_month;
  int m_day;
  int m_hour;
  int m_minute;
  int m_second;
  int m_timezone;
  char buf[32];

  const char *s_asctime(const void *ignore) const
  {
    time_t rawtime = now();
    struct tm *t = localtime(&rawtime);
    char *p = asctime(t);
    memset((void *)buf, 0, 32);
    if (p)
    {
      strncpy((char *)buf, p, 32);
      return (const char *)buf;
    }
    return NULL;
  }
};

#endif