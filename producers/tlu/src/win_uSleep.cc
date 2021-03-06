#ifdef WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif //NOMINMAX
#include <windows.h>
#include "tlu/win_Usleep.h"
#undef NOMINMAX

void uSleep(int waitTime) {
  __int64 time1 = 0, time2 = 0, freq = 0;

  QueryPerformanceCounter((LARGE_INTEGER *)&time1);
  QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

  do {
    QueryPerformanceCounter((LARGE_INTEGER *)&time2);
  } while ((time2 - time1) < waitTime);
}

#endif
