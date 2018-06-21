/*
 * log_utih.h
 *
 *  Created on: 2017年8月16日
 *      Author: LJT
 */

#ifndef LOG_UTIL_H_
#define LOG_UTIL_H_

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include "base.h"
#include "string_util.h"
#include "thread_util.h"

using std::string;

namespace Base {

#define PRINT_ERROR(fmt, args...) \
		Base::Logger::Instance()->Print(Base::Logger::LOG_ERR, "[%s - %d] " fmt, __FILE__, __LINE__, ##args)
#define PRINT_DEBUG(fmt, args...) \
		Base::Logger::Instance()->Print(Base::Logger::LOG_DEBUG, fmt, ##args)
#define PRINT_WARN(fmt, args...) \
		Base::Logger::Instance()->Print(Base::Logger::LOG_WARNING, fmt, ##args)
#define PRINT_INFO(fmt, args...) \
		Base::Logger::Instance()->Print(Base::Logger::LOG_INFO, fmt, ##args)

typedef void (*LogerHook)(const char* info);

class Logger {
public:
	typedef enum _LogLevel {
		LOG_EMERG = 0,
		LOG_ALERT = 1,
		LOG_CRIT = 2,
		LOG_ERR = 3,
		LOG_WARNING = 4,
		LOG_NOTICE = 5,
		LOG_INFO = 6,
		LOG_DEBUG = 7
	} LogLevel;

	static LogLevel Str2LogLevel(const char * str); 
	static const char * LogLevel2Str(LogLevel level); 
	static Logger * Instance(); 
	void Open(const char* filename, LogLevel level, uint64_t max_size); 
	void SetFile(const char* filename); 
	void SetLevel(LogLevel level); 
	void SetMaxSize(uint64_t max_size); 
	void Print(int level, const char* fmt, ...); 
	void SetHook(LogerHook hook);

private:
	Logger(); 
	Logger(const char* filename, LogLevel level, uint64_t max_size); 
	~Logger(); 

	void _Init(); 
	void PrintV(int level, const char* fmt, va_list args); 
	void _Close(); 
	bool _Open(); 
	void _DoPrintV(int level, const char* fmt, va_list args); 
	void _AppendTimestamp(std::string& time, const char* fmt); 
	const char* _LevelString(int level) const; 

private:
	FILE* 		_fp;
	string 		_file_name;
	LogLevel 	_level;
	uint64_t 	_max_size;
	PthreadMutex _mutex;
	LogerHook   _hook;
};

}

#endif
