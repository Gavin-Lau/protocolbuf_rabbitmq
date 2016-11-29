#ifndef _TIMER_H_
#define _TIMER_H_

#include <cstdint>

#if defined(_WIN32)
#include <windows.h>
inline uint64_t microsec_now(void)
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	return (((uint64_t)ft.dwHighDateTime << 32) | (uint64_t)ft.dwLowDateTime)
		/ 10;
}
#else
#include <sys/time.h>
inline uint64_t microsec_now(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000 + tv.tv_usec);
}

#endif

class MicroTimer{
public:
	MicroTimer() { mseconds = microsec_now(); }
	void reset() { mseconds = microsec_now(); }
	uint64_t expired() { return (microsec_now() - mseconds);  }
private:
	uint64_t  mseconds;
};

#endif _TIMER_H_