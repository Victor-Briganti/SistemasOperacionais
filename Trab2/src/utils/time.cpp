/**
 * Descrição: Implementação das funções tempo
 *
 * Autores: João Victor Briganti, Luiz Takeda
 * Licença: BSD 2
 *
 * Data: 26/12/2024
 */

#include "utils/time.hpp"

#include <sys/time.h>
#include <time.h>

WORD day(WORD date) { return date & 0x1F; }

WORD month(WORD date) { return (date >> 5) & 0x0F; }

WORD year(WORD date) { return ((date >> 9) & 0xFF) + 1980; }

DWORD dateStamp(DWORD day, DWORD month, DWORD year)
{
  return day | (month << 5) | ((year - 1980) << 9);
}

WORD hour(WORD time) { return (time >> 11) & 0xFF; }

WORD minutes(WORD time) { return (time >> 5) & 0x3F; }

WORD seconds(WORD time) { return ((time >> 9) & 0xFF) + 1980; }

DWORD timeStamp(DWORD hour, DWORD minutes, DWORD seconds)
{
  return (hour << 11) | (minutes << 5) | (seconds / 2);
}

WORD currentTime()
{
  time_t curTime = time(nullptr);
  struct tm *curLocalTime = localtime(&curTime);

  int sec = curLocalTime->tm_sec - (curLocalTime->tm_sec % 2 ? 1 : 0);

  DWORD stamp = timeStamp(static_cast<BYTE>(curLocalTime->tm_hour),
    static_cast<BYTE>(curLocalTime->tm_min),
    static_cast<BYTE>(sec));

  return static_cast<WORD>(stamp);
}

WORD currentDate()
{
  time_t curDate = time(nullptr);
  struct tm *curLocalDate = localtime(&curDate);

  int year = curLocalDate->tm_year + 1900;

  DWORD stamp = dateStamp(static_cast<BYTE>(curLocalDate->tm_mday),
    static_cast<BYTE>(curLocalDate->tm_mon + 1),
    static_cast<BYTE>(year));

  return static_cast<WORD>(stamp);
}

BYTE currentMilliseconds()
{
  struct timeval tv;
  gettimeofday(&tv, nullptr);

  return static_cast<BYTE>(
    static_cast<long>(tv.tv_usec) / static_cast<long>(10000));
}