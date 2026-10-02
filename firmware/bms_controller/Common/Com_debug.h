#ifndef __COM_DEBUG__
#define __COM_DEBUG__

#include "usart.h"
#include "stdio.h"
#include "stdarg.h"


#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE


#define debug_printf(format, ...) printf("[%s:%d]" format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)

#else

#define debug_printf(format, ...)
#endif // DEBUG

#endif // __COM_DEBUG__
