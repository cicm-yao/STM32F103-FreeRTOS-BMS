#include "App_display.h"

extern float bat_voltage;
extern float cell_current;
extern float bat_temp;
extern float bat_soc;

void App_display_init(void)
{
    Inf_OLED_Init();
}


void App_display_show(void)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        Inf_OLED_ShowChinese(24 + 16 * i, 0, i, 16, 1);
    }

    uint8_t buf[6] = {0};
    Inf_OLED_ShowString(0, 16, "V:", 16, 1);
    sprintf((char *)buf, "%.2f", bat_voltage);
    Inf_OLED_ShowString(20, 16, buf, 16, 1);

    Inf_OLED_ShowString(62, 16, "A:", 16, 1);
    memset(buf, 0, sizeof(buf));
    sprintf((char *)buf, "%.2f", cell_current);
    Inf_OLED_ShowString(80, 16, buf, 16, 1);

    Inf_OLED_ShowString(0, 32, "T:", 16, 1);
    memset(buf, 0, sizeof(buf));
    sprintf((char *)buf, "%.2f", bat_temp);
    Inf_OLED_ShowString(20, 32, buf, 16, 1);

    Inf_OLED_ShowString(62, 32, "SOC:", 16, 1);
    memset(buf, 0, sizeof(buf));
    sprintf((char *)buf, "%.0f", bat_soc);
    Inf_OLED_ShowString(94, 32, buf, 16, 1);

    taskENTER_CRITICAL();
    Inf_OLED_Refresh();

    taskEXIT_CRITICAL();
}
