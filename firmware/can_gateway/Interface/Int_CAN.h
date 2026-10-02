#ifndef __INT_CAN__
#define __INT_CAN__

#include "can.h"
#include "usart.h"

typedef struct
{
    uint16_t id;
    uint8_t data[8];
    uint8_t len;
}CAN_MSG_Struct;

/**
 * @brief 初始化CAN通信
 * 1. 配置过滤器  CAN通信过滤器是白名单机制 如果不配置过滤器 不接收任何消息
 * 2. CAN通信 低功耗模式 工作模式(手动启动CAN) 初始化模式
 *
 */
void Int_CAN_Init(void);

/**
 * @brief 发送数据
 * 
 * @param id 消息的ID
 * @param data 数据 最长8字节
 * @param len 长度
 */
void Int_CAN_Send(uint16_t id, uint8_t *data, uint8_t len);

/**
 * @brief CAN接收消息
 * 
 * @param msgs 存消息的队列
 * @param len 消息的条数
 */
void Int_CAN_Receive_Msgs(CAN_MSG_Struct *msgs, uint8_t *len);

#endif // __INT_CAN__
