/*
 * base.h
 *
 *  Created on: 2017年10月24日
 *      Author: LJT
 */

#ifndef _UTIL_BASE_H_
#define _UTIL_BASE_H_

#include "string_util.h"
#include "time_util.h"
#include "number_util.h"
#include "file_util.h"
#include "md5_util.h"
#include "list_util.h"
#include "sqlite_util.h"
#include "timer_util.h"
#include "log_util.h"
#include "thread_util.h"

uint16_t __builtin_bswap16(uint16_t a);
string ExecuteCmd(const char* cmdline, int* status = NULL);

#endif /* 3RD_BASE_UTIL_BASE_H_ */
