/*
 * sqlite_util.h
 *
 *  Created on: 2017年9月1日
 *      Author: LJT
 */

#ifndef COMMON_SQLITE_UTIL_H_
#define COMMON_SQLITE_UTIL_H_

#include <sqlite/sqlite3.h>
#include <string>
using std::string;

namespace Base {

class SqliteData {
public:
	SqliteData();
	~SqliteData(); 
	void Free(); 
	string Item(int row, int col); 

private:
	char **	_data;
	int 	_row;
	int 	_col;

	friend class SqliteUtil;
};

class SqliteUtil {
public:
	SqliteUtil(string path); 
	~SqliteUtil();  
	bool Open();  
	void Close();  
	bool Execute(const string &sql);  
	int64_t Query(const string &sql, int col_count, SqliteData& sqlite_data);  
	void Free(char **data);  

private:
	string 		_path;
	sqlite3 *	_conn;
	bool		_opened;
};

}

#endif /* COMMON_SQLITE_UTIL_H_ */
