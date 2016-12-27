#ifndef _MICROTIMER_H_
#define _MICROTIMER_H_

#if defined(_WIN32)
#include <windows.h>
inline unsigned long long microsec_now(void)
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	return (((unsigned long long)ft.dwHighDateTime << 32) | (unsigned long long)ft.dwLowDateTime)
		/ 10;
}
#else
#include <sys/time.h>
inline unsigned long long microsec_now(void)
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
	unsigned long long  expired() { return (microsec_now() - mseconds);  }
private:
	unsigned long long  mseconds;
};

#endif // _MICROTIMER_H_
