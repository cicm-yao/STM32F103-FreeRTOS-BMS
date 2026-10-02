#ifndef __APP_FREERTOS__
#define __APP_FREERTOS__

#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"
#include "Int_OLED.h"
#include "Int_BQ769.h"
#include "App_display.h"
#include "Int_CAN.h"

void App_freeRTOS_Start(void);

#endif // __APP_FREERTOS__
