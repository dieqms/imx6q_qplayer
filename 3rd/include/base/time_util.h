/*
 * time_util.h
 *
 *  Created on: 2017年8月18日
 *      Author: LJT
 */

#ifndef TIME_UTIL_H_
#define TIME_UTIL_H_

#include <string>
#include <sys/time.h>
#include "thread_util.h"

using std::string;

namespace Base {

class TimeUtil {
public:
	static int SetSysTime(time_t time); 
	static time_t Str2Time(const char * szTime);  
	static time_t Str2Time(const char * szTime, const char * fmt);  
	static string Time2Str(time_t time);  
	static string Time2Str(time_t time, const char * fmt);  

	static long int NowSeconds() {
		return time(NULL);
	}

	static long long int NowMillSeconds() {
		long long int now = 0;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		now = (long long int)((long long int)tv.tv_sec * 1000 + (long long int)tv.tv_usec / 1000);
		return now;
	}

	static long long int NowMicroSeconds() {
		long long int now = 0;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		now = (long long int)((long long int)tv.tv_sec * 1000000 + (long long int)tv.tv_usec);
		return now;
	}
};

class TimeMgr
{
public:
	static TimeMgr * Instance(); 
	
	void Sync(time_t newtime); 
	static time_t NowFromBoot(); 
	time_t BootTime(); 
	bool Synced(); 
	time_t Now(); 

private:
	TimeMgr();
	void Init(); 

	PthreadMutex _mutex;
	bool _synced;
	time_t _boot_time;
};

}

#endif /* SERVERCOMM_UTIL_H_ */
