/*
 * thread_util.h
 *
 *  Created on: 2017年8月16日
 *      Author: LJT
 */

#ifndef _THREAD_UTIL_H_
#define _THREAD_UTIL_H_

#include <list>
#include <string>
#include <sys/syscall.h>  
#include <pthread.h>
#include <string>
#include <errno.h>

namespace Base {

class PthreadMutexAttr {
public:
	PthreadMutexAttr(int type = -1, int shared = -1, int robust = -1,
					int proto = -1, int prioceiling = -1);
	~PthreadMutexAttr();
	operator const pthread_mutexattr_t*() const;
	operator pthread_mutexattr_t*();
	int Type() const;
	void Type(int val); 
	int Shared() const; 
	void Shared(int val); 
	int Protocol() const; 
	void Protocol(int val); 
	int PriorityCeiling() const; 
	void PriorityCeiling(int val); 
#ifdef PTHREAD_MUTEX_ROBUST
	int Robust() const;
	void Robust(int val);
#endif

private:
	pthread_mutexattr_t _attr;
};

class PthreadMutex {
public:
	PthreadMutex(const pthread_mutexattr_t* attr = NULL); 
	~PthreadMutex();
	void lock();
	bool try_lock();
	void unlock();
	void wait();
	bool wait(int wait_ms);
	void signal();
	void broadcast(); 
private:
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
};

class PthreadInfo {
public:
	unsigned int id;
	std::string name;
};

typedef void (*runner_t)(void* _args);

class Pthread {
public:
	Pthread(runner_t runner, void * args = NULL, std::string name = "Unknown");
	virtual ~Pthread();
	void set_stacksize(size_t stacksize = 1 * 1024 * 1024);
	bool start(size_t stacksize = 1 * 1024 * 1024); 
	void join(); 
	void stop(bool with_block = false); 
	bool is_run(); 
	static unsigned int get_tid();
	static void dump();
	std::string name();
	pthread_t handle();

protected:
	virtual void runner(void * var); 

private:
	static void * loop(void * var); 

protected:
	pthread_t 	_thread_handle;
	size_t 		_stacksize;
	std::string _thread_name;
	bool 		_running;
	runner_t 	_runner;
	void * 		_args;
	
	static		PthreadMutex			 mutex;
	static		std::list<PthreadInfo> pthread_list;
};

}

#endif
