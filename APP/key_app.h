//
// Created by 13615 on 2025/12/11.
//

#ifndef SMART_CAR_KEY_APP_H
#define SMART_CAR_KEY_APP_H

#include "main.h"
#include <stdint.h>
#include "bsp_system.h"
/* ==================== 硬件配置 ==================== */
#define KEY_GPIO_PORT           GPIOE
#define KEY1_PIN                KEY1_Pin        // PE0
#define KEY2_PIN                KEY2_Pin        // PE1
#define KEY3_PIN                KEY3_Pin        // PE2
#define KEY4_PIN                KEY4_Pin        // PE3

/* 按键电平定义（上拉输入，按下为低） */
#define KEY_PRESSED_LEVEL       GPIO_PIN_RESET
#define KEY_RELEASED_LEVEL      GPIO_PIN_SET

/* ==================== 按键参数配置 ==================== */
#define KEY_DEBOUNCE_MS         20              // 消抖时间(ms)
#define KEY_LONG_PRESS_MS       1000            // 长按阈值(ms)
#define KEY_SCAN_INTERVAL_MS    10              // 扫描间隔(ms)

/* ==================== 按键索引 ==================== */
typedef enum {
    KEY_ID_1 = 0,
    KEY_ID_2,
    KEY_ID_3,
    KEY_ID_4,
    KEY_NUM_MAX
} Key_ID_t;

/* ==================== 按键事件 ==================== */
typedef enum {
    KEY_EVENT_NONE = 0,         // 无事件
    KEY_EVENT_SHORT_PRESS,      // 短按（松开时触发）
    KEY_EVENT_LONG_PRESS,       // 长按（达到阈值时触发）
    KEY_EVENT_PRESSING          // 按下中（持续按住）
} Key_Event_t;

/* ==================== 按键状态 ==================== */
typedef enum {
    KEY_STATE_IDLE = 0,         // 空闲
    KEY_STATE_DEBOUNCE,         // 消抖中
    KEY_STATE_PRESSED,          // 已按下
    KEY_STATE_LONG_PRESSED      // 长按已触发
} Key_State_t;

/* ==================== 按键结构体 ==================== */
typedef struct {
    GPIO_TypeDef    *port;
    uint16_t        pin;
    Key_State_t     state;
    uint32_t        press_tick;     // 按下时刻
    uint8_t         long_triggered; // 长按已触发标志
} Key_t;

/* ==================== 函数声明 ==================== */
void Key_Init(void);
void Key_Scan(void);                            // 周期调用(建议10ms)
Key_Event_t Key_GetEvent(Key_ID_t id);          // 获取按键事件(获取后自动清除)
uint8_t Key_IsPressed(Key_ID_t id);             // 实时检测按键是否按下
void key_proc(void);
#endif //SMART_CAR_KEY_APP_H
