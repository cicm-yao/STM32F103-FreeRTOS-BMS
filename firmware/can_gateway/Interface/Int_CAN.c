#include "Int_CAN.h"

/**
 * @brief 初始化CAN通信
 * 1. 配置过滤器  CAN通信过滤器是白名单机制 如果不配置过滤器 不接收任何消息
 * 2. CAN通信 低功耗模式 工作模式(手动启动CAN) 初始化模式
 *
 */
void Int_CAN_Init(void)
{
    // 1. 配置过滤器
    CAN_FilterTypeDef sFilterConfig = {0};

    sFilterConfig.FilterBank = 0;                      // 过滤器编号
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;  // 标识符屏蔽模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; // 32位过滤器
    sFilterConfig.FilterIdHigh = 0x0000;               // 标识符高16位
    sFilterConfig.FilterIdLow = 0x0000;                // 标识符低16位
    sFilterConfig.FilterMaskIdHigh = 0x0000;           // 标识符屏蔽高16位
    sFilterConfig.FilterMaskIdLow = 0x0000;            // 标识符屏蔽低16位
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; // 过滤器关联的FIFO
    sFilterConfig.FilterActivation = ENABLE;           // 使能过滤器

    HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);

    // 2. 启动CAN通信 => 进入到正常工作模式
    HAL_CAN_Start(&hcan);
}

/**
 * @brief 发送数据
 *
 * @param id 消息的ID
 * @param data 数据 最长8字节
 * @param len 长度
 */
void Int_CAN_Send(uint16_t id, uint8_t *data, uint8_t len)
{
    // 1. 判断邮箱空闲
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0)
    {
        return;
    }

    // 2. 构造消息
    CAN_TxHeaderTypeDef TxMessage = {0};
    TxMessage.StdId = id;         // 标准ID
    TxMessage.IDE = CAN_ID_STD;   // 标准帧
    TxMessage.RTR = CAN_RTR_DATA; // 数据帧
    TxMessage.DLC = len;          // 数据长度
    uint32_t TxMailbox = 0;
    HAL_CAN_AddTxMessage(&hcan, &TxMessage, data, &TxMailbox);
}

/**
 * @brief CAN接收消息
 *
 * @param msgs 存消息的队列
 * @param len 消息的条数
 */
void Int_CAN_Receive_Msgs(CAN_MSG_Struct *msgs, uint8_t *len)
{
    // 1. 判断是否有消息
    *len = HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0);
    if (*len == 0)
    {
        return;
    }

    // 2. 读取消息
    for (uint8_t i = 0; i < *len; i++)
    {
        CAN_RxHeaderTypeDef RxMessage;
        HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxMessage, msgs[i].data);
        msgs[i].id = RxMessage.StdId;
        msgs[i].len = RxMessage.DLC;
    }
}
