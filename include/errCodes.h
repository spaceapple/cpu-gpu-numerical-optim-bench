
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/


#ifndef _ERR_CODES_H
#define _ERR_CODES_H


#ifdef __cplusplus
namespace Common
{
#endif

typedef enum ErrCode_t
{
    NoError                      = 0,
    UnknownError                 = 1,
    IOCantOpenFile               = 2,
    AnnotInfoParsingError        = 3,
    InvalidTemplateDimension     = 4,

} ErrCode;


#ifdef __cplusplus
} // end namespace Common
#endif

#endif /* _ERR_CODES_H *  * */


