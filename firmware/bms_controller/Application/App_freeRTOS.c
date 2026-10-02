#include "App_freeRTOS.h"

float cell_voltage[5] = {0};
float bat_voltage = 0;
float cell_current = 0;
float bat_temp = 0;
float bat_soc = 0;

CAN_MSG_Struct CAN_msg[3];
uint8_t msg_count = 0;

void OLED_task(void *pvParameters);
#define OLED_TASK_PRIO 1
#define OLED_TASK_STACK_SIZE 128
TaskHandle_t OLED_task_handle;


void Cell_task(void *pvParameters);
#define CELL_TASK_PRIO 4
#define CELL_TASK_STACK_SIZE 128
TaskHandle_t Cell_task_handle;


void Temp_task(void *pvParameters);
#define TEMP_TASK_PRIO 3
#define TEMP_TASK_STACK_SIZE 128
TaskHandle_t Temp_task_handle;

void Bat_task(void *pvParameters);
#define BAT_TASK_PRIO 2
#define BAT_TASK_STACK_SIZE 128
TaskHandle_t Bat_task_handle;

void CAN_task(void *pvParameters);
#define CAN_TASK_PRIO 4
#define CAN_TASK_STACK_SIZE 128
TaskHandle_t CAN_task_handle;


void App_freeRTOS_Start(void)
{

    xTaskCreate(OLED_task, "OLED_task", OLED_TASK_STACK_SIZE, NULL, OLED_TASK_PRIO, &OLED_task_handle);
    xTaskCreate(Cell_task, "Cell_task", CELL_TASK_STACK_SIZE, NULL, CELL_TASK_PRIO, &Cell_task_handle);

    xTaskCreate(Temp_task, "Temp_task", TEMP_TASK_STACK_SIZE, NULL, TEMP_TASK_PRIO, &Temp_task_handle);
    xTaskCreate(Bat_task, "Bat_task", BAT_TASK_STACK_SIZE, NULL, BAT_TASK_PRIO, &Bat_task_handle);

    xTaskCreate(CAN_task, "CAN_task", CAN_TASK_STACK_SIZE, NULL, CAN_TASK_PRIO, &CAN_task_handle);
    vTaskStartScheduler();
}

void OLED_task(void *pvParameters)
{
    App_display_init();
    while (1)
    {
        App_display_show();
        vTaskDelay(1000);
    }
}

void Cell_task(void *pvParameters)
{

    Int_BQ769_Init();
    while (1)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
            cell_voltage[i] = Int_BQ769_Get_Cell_Voltage(i + 1);
            // debug_printf("cell%d:%.2f\r\n", i + 1, cell_voltage[i]);
        }

        bat_voltage = Int_BQ769_Get_Bat_Voltage();
        // debug_printf("bat:%.2f\r\n", bat_voltage);
        cell_current = Int_BQ769_Get_Bat_Current();
        debug_printf("current:%.2f\r\n", cell_current);

        vTaskDelay(1000);
    }
}

void Temp_task(void *pvParameters)
{
    while (1)
    {
        bat_temp = Int_BQ769_Get_Bat_temp();
        debug_printf("temp:%.2f\r\n", bat_temp);
        vTaskDelay(1000);
    }
}

void Bat_task(void *pvParameters)
{
    while (1)
    {
        float sum_soc = 0;
        for (uint8_t i = 0; i < 5; i++)
        {
            sum_soc += Com_BQ769_VolToSoc(cell_voltage[i] * 1000);
        }
        float tmp_soc = sum_soc / 5;
        if (bat_temp < 40)
        {
            bat_soc = tmp_soc * (1 - (bat_temp - 25) * 0.005);
        }
        else
        {
            bat_soc = tmp_soc * (1 - (((40 - 25) * 0.005) + ((bat_temp - 40) * 0.008)));
        }

        debug_printf("soc:%.2f\r\n", bat_soc);
        vTaskDelay(1000);
    }
}

void CAN_task(void *pvParameters)
{
    Int_CAN_Init();
    while (1)
    {
        Int_CAN_Receive_Msgs(CAN_msg, &msg_count);
        if (msg_count > 0)
        {
            for (uint8_t i = 0; i < msg_count; i++)
            {
                debug_printf("CAN%d:%s\r\n", i, CAN_msg[i].data);
 ָ��               if (CAN_msg[i].data[0] == '0')
                {
        
                    if (CAN_msg[i].data[1] == '1')
                    {
    
                        Int_BQ769_discharge(1);
                    }
                    else if (CAN_msg[i].data[1] == '0')
                    {
               
                        Int_BQ769_discharge(0);
                    }
                }
                else if (CAN_msg[i].data[0] == '1')
                {
       
                    if (CAN_msg[i].data[1] == '1')
                    {
                
                        Int_BQ769_charge(1);
                    }
                    else if (CAN_msg[i].data[1] == '0')
                    {
                
                        Int_BQ769_charge(0);
                    }
                }
            }
            msg_count = 0;
        }
        vTaskDelay(500);
    }
}
