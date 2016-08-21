#ifndef _COMMON_TOOLS_H_
#define _COMMON_TOOLS_H_


#define IS_VALID_PCHAR(pchar)		(NULL != pchar && 0 != pchar[0]) 
#define IS_VALID_NUM_0EXCEPT(val)	(val > 0)
#define IS_VALID_NUM_0INCLUDE(val)	(val >= 0)
#define IS_VALID_ALPHABET(ch)		( (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') )
#define IS_FORMAT_CH(ch)			(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')

#ifdef __cplusplus
# if defined(__FUNCTION__)
#  define POS_STRING_CPP				("FILE:" + __FILE__ + " FUNC:" + __FUNCTION__ + "LINE:" + __LINE__)
# elif defined(__func__)
#  define POS_STRING_CPP				("FILE:" + __FILE__ + " FUNC:" + __func__ + "LINE:" + __LINE__)
# else
#  define POS_STRING_CPP				("FILE:" + __FILE__ + "LINE:" + __LINE__)
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif



#endif // _COMMON_TOOLS_H_