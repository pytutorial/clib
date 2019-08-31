#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "string_util.h"
#include "object.h"

inline static void _check_valid_string(String st, const char* file_name, int line)
{
	if(st._state != VALID || st.scope->state != VALID)
	{		
		QUIT("Access invalid string", file_name, line);
	}
}

int str_len(String st)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	return st._len;
}

char* str_data(String st)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	
	return st._data;
}

String new_nstring(Scope* scope, const char* src, int len)
{
	char* data = zero_alloc(scope, len + 1);
    strncpy(data, src, len);
    data[len] = 0;
	return (String){data, len, VALID, scope};
}

String new_string(Scope* scope, const char* src)
{
    return new_nstring(scope, src, strlen(src));
}

char* _vsfmt(Scope* scope, const char* fmt, va_list arglist, int* out_length)
{
    int len = vsnprintf(NULL, 0, fmt, arglist);        
    char* data = zero_alloc(scope, len + 1);
    vsprintf(data, fmt, arglist);
    *out_length = len;
    return data;
}

String new_string_fmt(Scope* scope, const char* fmt, ...)
{
    va_list arglist;    
    va_start(arglist, fmt );
    int len;
    char* data = _vsfmt(scope, fmt, arglist, &len);
    va_end(arglist);
	
	return (String) {data, len, VALID, scope };
}

String _concat_string_with_ptr(String st, char* p, int len)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	
	int stlen = st._len;
	
	char* data = zero_alloc(st.scope, stlen + len + 1);
	
    strncpy(data, st._data, stlen);
	strncpy(data + stlen, p, len);
	
    data[stlen + len] = 0;
	
	return (String){data, stlen + len, VALID, st.scope};
}

String concat_string_with_ptr(String st, char* p)
{
	return _concat_string_with_ptr(st, p, strlen(p));
}

String concat_string(String st1, String st2)
{
	_check_valid_string(st1, __FILE__,  __LINE__);
	
	return _concat_string_with_ptr(st1, st2._data, st2._len);
}

String sub_string(String st, int start, int end)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	
    if(end > st._len)
        end = st._len;
    
    if(start < 0 || end <= start)
        return new_string(st.scope, "");
    
    return new_nstring(st.scope, st._data + start, end - start);    
}

String right_string(String st, int start)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	
    return sub_string(st, start, st._len);
}

String left_string(String st, int end)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	
    return sub_string(st, 0, end);
}

int find_char(String st, char needle)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	
    char * pos = NULL;
	char * data = st._data;
    
    if(data)
        pos = strchr(data, needle);
    
    return pos? pos - data : -1;
}

int find_string(String st, const char* needle)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	
    char * pos = NULL;
	char * data = st._data;
    
    if(data)
        pos = strstr(data, needle);
    
    return pos? pos - data : -1;
}

ListString split_string(String st, const char* delimiter)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	
    ListString v = new_list_string(st.scope, 0);
    
    int delimiter_len = strlen(delimiter);
    
    char * ptr = st._data;
    char * pos;
    
    while((pos = strstr(ptr, delimiter)))
    {
        int len = pos - ptr;
        if(len > 0)        
            push(v, new_nstring(st.scope, ptr, len));
        
        ptr = pos + delimiter_len;
    }
    
    if(*ptr)
        push(v, new_string(st.scope, ptr));
    
	return v;
}

bool string_equal(String st1, String st2)
{
    return st1._len == st2._len && strncmp(st1._data, st2._data, st1._len) == 0;
}

void print_string(String st)
{
	_check_valid_string(st, __FILE__,  __LINE__);
	
    puts(st._data);
}