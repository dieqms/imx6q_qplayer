/*
 * string_util.h
 *
 *  Created on: 2017年8月18日
 *      Author: LJT
 */
#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <string>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
using std::string;

namespace Base {

template<typename T>
struct StringJoinConvert
{
	T operator()(T v) { return v; }
};

class StringUtil {
public:
	static bool IsNullOrSpace(const char* str, int len = -1); 

	static bool IsNullOrEmpty(const char* str) ;

	static string& AppendFormat(string& str, const char* fmt, ...) ;

	static string Format(const char* fmt, ...) ;

	static string& Format(string& str, const char* fmt, ...) ;


	static void ToHexStr(const void* in, int inLen, char* out, int outLen,
			bool upperCase = false); 

	static string ToHexStr(const void* in, int inLen, bool upperCase = false); 

	static void FromHexStr(const string& in, unsigned char *out, size_t outLen) ;

	static void FromHexStr(const char *in, size_t inLen, unsigned char* out, size_t outLen) ;

	// print binary in hex string way
	static string HexDump(const void* data, int len, int lineLength = 16,
			bool upperCase = true) ;

	static string& TrimLeft(string& str, const char* ch = ""); 

	static char* TrimLeft(char* str, size_t len, const char* ch = "") ;

	static string& TrimRight(string& str, const char* ch = "") ;

	static char* TrimRight(char* str, size_t len, const char* ch = "") ;

	static string& Trim(string& str, const char* ch = "") ;

	static char* Trim(char* str, size_t len, const char* ch = "") ;

	static char* MakeLower(char* str, int len = -1); 

	static string& MakeUpper(string& str) ;

	static string& MakeLower(string& str) ;

	static int Compare(const void* s1, const void* s2, size_t len,
			bool ignoreCase = false) ;

	static bool StartsWith(const char* str, const char* substr,
			bool ignoreCase = false) ;

	static bool StartsWith(const char* str, size_t len,
			const char* substr, bool ignoreCase = false) ;

	static bool StartsWith(const char* str, size_t len,
			const char* substr, size_t subLen, bool ignoreCase = false) ;

	static bool EndsWith(const char* str, const char* substr,
			bool ignoreCase = false) ;

	static bool EndsWith(const char* str, size_t len, const char* substr,
			bool ignoreCase = false) ;

	static bool EndsWith(const char* str, size_t len, const char* substr,
			size_t subLen, bool ignoreCase = false) ;

	static bool Contains(const char* str, const char* substr,
			bool ignoreCase = false) ;

	static bool Contains(const string& str,
			const string& substr, bool ignoreCase = false) ;

	static bool Contains(const char* str, size_t len, const char* substr,
			bool ignoreCase = false) ;

	static bool Contains(const char* str, size_t len, const char* substr,
			size_t subLen, bool ignoreCase = false) ;

	static char* Find(const char* str, const char* substr,
			bool ignoreCase = false) ;

	static char* Find(const char* str, size_t len, const char* substr,
			bool ignoreCase = false) ;

	static char* Find(const char* str, size_t len, const char* substr,
			size_t subLen, bool ignoreCase) ;
	
	/**
	 * 分割字符串
	 * @template param T 支持T::insert(T::end(), std::string)方法的任意容器
	 * @param output	输出分割后的结果数组
	 * @param input		输入字符串
	 * @param delim		分隔字符串
	 * @param remove_empty	从输出结果中删除空白串
	 * @param trim		去除每个字符串前后的空白
	 * @return 返回结果数组output
	 **/
	template<typename T>
	static T& StringSplit(
		T& output,
		const char* input,
		const char* delim,
		bool remove_empty = false,
		bool trim = false)
	{
		if(input == 0 || *input == 0 || delim == 0 || *delim == 0)
		{
			return output;
		}

		std::string str;
		const char* start = input;
		const char* p = 0;
		while ((p = strstr(start, delim)) != 0)
		{
			if(!remove_empty || !IsNullOrSpace(start, p - start))
			{
				str.assign(start, p - start);
				if(trim)
				{
					Trim(str);
				}
				
				if(!remove_empty || !str.empty())
				{
					output.insert(output.end(), str);
				}
			}
			start = p + strlen(delim);
		}

		if (!remove_empty || !IsNullOrSpace(start))
		{
			str.assign(start);
			if (trim)
			{
				Trim(str);
			}
			if(!remove_empty || !str.empty())
			{
				output.insert(output.end(), str);
			}
		}
		return output;
	}

   	template<typename T>
	static T& StringSplit(
		T& output,
		const std::string& input,
		const char* delim,
		bool remove_empty = false,
		bool trim = false)
    {
        return StringSplit(output, input.c_str(), delim, remove_empty, trim);
    }

	/**
	 * 拼接字符串
	 * @param begin	起始位置
	 * @param end	结束位置
	 * @param separator	分隔符
	 * @param fmt	元素格式化串
	 * @return 返回拼接后的字符串
	 **/
	template<
		typename Iterator,
		typename TConv = StringJoinConvert<typename Iterator::value_type>
	>
	static std::string StringJoin(
		Iterator begin,
		Iterator end,
		const char* separator,
		const char* fmt = "%s",
		TConv conv = TConv())
	{
		std::string result;
		Iterator it = begin;
		
		//append first element
		if(it != end)
		{
			AppendFormat(result, fmt, conv(*it));
			++it;
		}
		
		//append other element
		for(; it != end; ++it)
		{
			result.append(separator);
			AppendFormat(result, fmt, conv(*it));
		}

		return result;
	}
    
	//specialized for array
	template<
		typename T,
		typename TConv = StringJoinConvert<T>
	>
	static std::string StringJoin(
		T* begin,
		T* end,
		const char* separator,
		const char* fmt = "%s",
		TConv conv = TConv())
	{
		return StringJoin<T*, TConv>(begin, end, separator, fmt, conv);
	}

	/**
	 * 拼接字符串
	 * @param cont	字符串所在容器
	 * @param separator	分隔符
	 * @param fmt	元素格式化串
	 * @return 返回拼接后的字符串
	 **/
	template<
		typename TContainer,
		typename TConv = StringJoinConvert<typename TContainer::value_type>
	>
	static std::string StringJoin(
		TContainer cont,
		const char* separator,
		const char* fmt = "%s",
		TConv conv = TConv())
	{
		return StringJoin(cont.begin(), cont.end(), separator, fmt, conv);
	}

	//specialized for array
	template<
		class T,
		size_t N,
		typename TConv = StringJoinConvert<T>
	>
	static std::string StringJoin(
		T (&cont)[N],
		const char* separator,
		const char* fmt = "%s",
		TConv conv = TConv())
	{
		return StringJoin<T, TConv>(&cont[0], &cont[N], separator, fmt, conv);
	}


private:
	static string& _AppendFormatV(string& str, const char* fmt, va_list ap); 
	static char ToHex(uint8_t num, bool upperCase = false);
	static uint8_t FromHex(char ch);
};

}

#endif
