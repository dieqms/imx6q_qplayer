/*
 * json_util.h
 *
 *  Created on: 2017年12月27日
 *      Author: LJT
 */

#ifndef _JSON_UTIL_H_
#define _JSON_UTIL_H_

#include <string>
#include "parson.h"
using std::string;

namespace Base {

class JsonArray {
public:
	JsonArray(JSON_Array * array = NULL) {
		_array = array;
	}
	
	void Assign(JSON_Array * array) {
		_array = array;
	}
	
	size_t GetCount() {
		return GetArrayCount(_array);
	}
	
	bool GetBool(size_t index, bool & val) {
		return GetArrayBool(_array, index, val);
	}
	
	template<typename T>
	bool GetNumber(size_t index, T & val) {
		return GetArrayNumber(_array, index, val);
	}

	bool GetString(size_t index, string & val) {
		return GetArrayString(_array, index, val);
	}

	static size_t GetArrayCount(const JSON_Array *array) {
		return json_array_get_count(array);
	}
	
	static bool GetArrayBool(const JSON_Array *array, size_t index, bool & val) {
		int ret = json_array_get_boolean(array, index);
		if (ret == -1) {
			printf("[json_util] get array %d fail\n", index);
			return false;
		}
		val = ret;
		return true;
	}

	template<typename T>
	static bool GetArrayNumber(const JSON_Array *array, size_t index, T & val) {
		if (NULL == json_array_get_value(array, index)) {
			printf("[json_util] get array %d fail\n", index);
			return false;
		}
		
		double ret = json_array_get_number(array, index);
		val = ret;
		return true;
	}

	static bool GetArrayString(const JSON_Array *array, size_t index, string & val) {
		const char * string_ret = json_array_get_string(array, index);
		if (string_ret == NULL) {
			printf("[json_util] get array %d fail\n", index);
			return false;
		}

		val.assign(string_ret);
		return true;
	}

private:
	JSON_Array * _array;
};

class JsonUtil {
public:
	JsonUtil() {
		_root_value = NULL;
		_commit = NULL;
    }

    ~JsonUtil() {
    	Close();
    }
    
    bool CreateObject() {
    	/* parsing json and validating output */
    	_root_value = json_value_init_object();
    	if (NULL == _root_value) {
    		printf("[json_util] json_value_init_object fail!\n");
    		return false;
    	}

    	_commit = json_value_get_object(_root_value);
    	
    	return true;
    }

    bool ParseFile(const char* file) {
    	_root_value = json_parse_file_with_comments(file);
		if (NULL == _root_value) {
			printf("[json_util] parse file %s fail!\n", file);
			return false;
		}
		
		_commit = json_value_get_object(_root_value);

        return true;
    }
    
    bool SaveFile(const char* file) {
    	JSON_Status ret = JSONSuccess;
    	ret = json_serialize_to_file_pretty(_root_value, file);
    	if (ret != JSONSuccess) {
    		printf("[json_util] save file %s fail!\n", file);
    		return false;
    	}
    	
    	return true;
    }
    
    bool SaveString(std::string & str) {
    	bool ret = false;
    	char * out = json_serialize_to_string_pretty(_root_value);
    	
    	if (out != NULL) {
    		str = out;
    		json_free_serialized_string(out);
    		ret = true;
    	}
    	
    	return ret;
    }

	void Close() {
		if (_root_value) {
			json_value_free(_root_value);
			_root_value = NULL;
			_commit = NULL;
		}
	}
	
	bool GetBool(const char* key, bool & val) {
		return GetJsonBool(_commit, key, val);
	}
	
	template<typename T>
	bool GetNumber(const char* key, T & val) {
		return GetJsonNumber(_commit, key, val);
	}

    bool GetString(const char* key, string & val) {
    	return GetJsonString(_commit, key, val);
    }
    
	bool GetArray(const char* key, JsonArray & val) {
    	JSON_Array * array = json_object_dotget_array(_commit, key);
    	if (array == NULL) {
       		printf("[json_util] get %s fail\n", key);
    		return false;
    	}
    	
    	val.Assign(array);
		return true;
	}
    
    bool SetString(const char* key, const string & val) {
		return SetJsonString(_commit, key, val);
	}

	template<typename T>
	bool SetNumber(const char* key, const T & val) {
		return SetJsonNumber(_commit, key, val);
	}

	bool SetBool(const char* key, bool val) {
		return SetJsonBool(_commit, key, val);
	}
    
    static bool GetJsonBool(JSON_Object* commit, const char* key, bool & val) {
    	int ret = json_object_dotget_boolean(commit, key);
    	if (ret == -1) {
    		printf("[json_util] get %s fail\n", key);
    		return false;
    	}
    	val = ret;
    	return true;
    }

    template<typename T>
    static bool GetJsonNumber(JSON_Object* commit, const char* key, T & val) {
        if (!json_object_dothas_value(commit, key)) {
    		printf("[json_util] get %s fail\n", key);
            return false;
        }

    	double ret = json_object_dotget_number(commit, key);	
    	val = ret;
    	return true;
    }

    static bool GetJsonString(JSON_Object* commit, const char* key, string & val) {
    	const char * string_ret = json_object_dotget_string(commit, key);
    	if (string_ret == NULL) {
    		printf("[json_util] get %s fail\n", key);
    		return false;
    	}
    	
    	val.assign(string_ret);
    	return true;
    }

    static bool SetJsonString(JSON_Object* commit, const char* key, const string & val) {
    	int ret = json_object_dotset_string(commit, key, val.c_str());
    	if (ret != JSONSuccess) {
    		printf("[json_util] save %s fail\n", key);
    		return false;
    	}
    	
    	return true;
    }

    template<typename T>
    static bool SetJsonNumber(JSON_Object* commit, const char* key, const T & val) {
    	int ret = json_object_dotset_number(commit, key, val);
    	if (ret != JSONSuccess) {
    		printf("[json_util] save %s fail\n", key);
    		return false;
    	}
    	
    	return true;
    }

    static bool SetJsonBool(JSON_Object* commit, const char* key, bool val) {
    	int ret = json_object_dotset_boolean(commit, key, val);
    	if (ret != JSONSuccess) {
    		printf("[json_util] save %s fail\n", key);
    		return false;
    	}
    	
    	return true;
    }

private:
	JSON_Value * _root_value;
	JSON_Object* _commit;
};

}

#endif /* SRC_3RD_INCLUDE_BASE_BASE_EXCEL_UTIL_H_ */
