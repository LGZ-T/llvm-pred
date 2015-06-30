//#include <iostream>
#include <map>
#include <string>
//#include <time.h>
//#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#define CPUT 0.416739

using namespace std;
typedef unsigned long long ulonglong;

static ulonglong elapstime=0;
static ulonglong lasttime=0;
static inline ulonglong timing(void)
{
#ifdef __i386__
	unsigned long long s;
	asm volatile("rdtsc" : "=A" (s) :: "memory");
	return s;
#else
	unsigned low, high;
	asm volatile("rdtsc" : "=a" (low), "=d" (high) :: "memory");
	return ((unsigned long long)high << 32) | low;
#endif
}


uint64_t timing_err()
{
    ulonglong a, b;
    a=timing();
    b=timing();
    return b-a;
}

extern "C"
{
    void gettime();
    void outinfo();
}

void gettime()
{
    ulonglong curtime = timing();
    elapstime = curtime-lasttime-timing_err();
    lasttime = curtime;
}
void outinfo()
{
    fprintf(stderr, "elapsed cycles:%llu\n",elapstime);
}

