/*
 * excel_util.h
 *
 *  Created on: 2017年12月27日
 *      Author: LJT
 */

#ifndef _EXCEL_UTIL_H_
#define _EXCEL_UTIL_H_

#include "xlsxwriter.h"

namespace Base {

class ExcelWriter {
public:
    ExcelWriter() {
        workbook = NULL;
        worksheet = NULL;
    }

    ~ExcelWriter() {
    	Close();
    }

    bool Open(const char* book, const char * sheet) {
        /* Create a workbook and add a worksheet. */
        workbook  = workbook_new(book);
        if (NULL == workbook) {
            PRINT_ERROR("Fail open excel %s\n", book);
            return false;
        }

        worksheet = workbook_add_worksheet(workbook, sheet);
        if (NULL == worksheet) {
            PRINT_ERROR("Fail open excel sheet %s\n", sheet);
            return false;
        }

        return true;
    }

    bool Write(int row, int col, const char* val) {
        return LXW_NO_ERROR == worksheet_write_string(worksheet, row, col, val, NULL) ? true : false;
    }

    bool Write(int row, int col, int val) {
    	return LXW_NO_ERROR == worksheet_write_number(worksheet, row, col, val, NULL) ? true : false;
    }
    
    bool Close() {
    	bool ret = true;
    	
    	 if (workbook) {
    		 ret = LXW_NO_ERROR == workbook_close(workbook) ? true : false;
    		 workbook = NULL;
    		 worksheet = NULL;
    	 }
    	 
    	 return ret;
    }

private:
    lxw_workbook  * workbook;
    lxw_worksheet * worksheet;
};

}

#endif /* SRC_3RD_INCLUDE_BASE_BASE_EXCEL_UTIL_H_ */
