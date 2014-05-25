#ifndef __PARSER_H_
#define __PARSER_H_
#include "parser.h"
#endif


unsigned WindowsTickToUnixSeconds(long long windowsTicks)
{
     return (unsigned)(windowsTicks / WINDOWS_TICK - SEC_TO_UNIX_EPOCH);
}