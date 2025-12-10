//
// Created by 13615 on 2025/12/11.
//

#include "key_app.h"

/* ==================== 私有变量 ==================== */
static Key_t s_keys[KEY_NUM_MAX];
static Key_Event_t s_key_events[KEY_NUM_MAX];

/* ==================== 按键初始化 ==================== */
void Key_Init(void)
{
    s_keys[KEY_ID_1].port = KEY_GPIO_PORT;
    s_keys[KEY_ID_1].pin  = KEY1_PIN;

    s_keys[KEY_ID_2].port = KEY_GPIO_PORT;
    s_keys[KEY_ID_2].pin  = KEY2_PIN;

    s_keys[KEY_ID_3].port = KEY_GPIO_PORT;
    s_keys[KEY_ID_3].pin  = KEY3_PIN;

    s_keys[KEY_ID_4].port = KEY_GPIO_PORT;
    s_keys[KEY_ID_4].pin  = KEY4_PIN;

    for (uint8_t i = 0; i < KEY_NUM_MAX; i++) {
        s_keys[i].state = KEY_STATE_IDLE;
        s_keys[i].press_tick = 0;
        s_keys[i].long_triggered = 0;
        s_key_events[i] = KEY_EVENT_NONE;
    }
}

/* ==================== 读取按键电平 ==================== */
static inline uint8_t Key_ReadPin(Key_ID_t id)
{
    return (HAL_GPIO_ReadPin(s_keys[id].port, s_keys[id].pin) == KEY_PRESSED_LEVEL);
}

/* ==================== 按键扫描(周期调用) ==================== */
void Key_Scan(void)
{
    uint32_t now = HAL_GetTick();

    for (uint8_t i = 0; i < KEY_NUM_MAX; i++) {
        uint8_t pressed = Key_ReadPin((Key_ID_t)i);
        Key_t *key = &s_keys[i];

        switch (key->state) {
            case KEY_STATE_IDLE:
                if (pressed) {
                    key->state = KEY_STATE_DEBOUNCE;
                    key->press_tick = now;
                }
                break;

            case KEY_STATE_DEBOUNCE:
                if (!pressed) {
                    key->state = KEY_STATE_IDLE;
                } else if ((now - key->press_tick) >= KEY_DEBOUNCE_MS) {
                    key->state = KEY_STATE_PRESSED;
                    key->long_triggered = 0;
                }
                break;

            case KEY_STATE_PRESSED:
                if (!pressed) {
                    if (!key->long_triggered) {
                        s_key_events[i] = KEY_EVENT_SHORT_PRESS;
                    }
                    key->state = KEY_STATE_IDLE;
                } else if ((now - key->press_tick) >= KEY_LONG_PRESS_MS) {
                    if (!key->long_triggered) {
                        key->long_triggered = 1;
                        s_key_events[i] = KEY_EVENT_LONG_PRESS;
                    }
                    key->state = KEY_STATE_LONG_PRESSED;
                }
                break;

            case KEY_STATE_LONG_PRESSED:
                if (!pressed) {
                    key->state = KEY_STATE_IDLE;
                }
                break;

            default:
                key->state = KEY_STATE_IDLE;
                break;
        }
    }
}

/* ==================== 获取按键事件(获取后清除) ==================== */
Key_Event_t Key_GetEvent(Key_ID_t id)
{
    if (id >= KEY_NUM_MAX) return KEY_EVENT_NONE;

    Key_Event_t event = s_key_events[id];
    s_key_events[id] = KEY_EVENT_NONE;
    return event;
}

/* ==================== 实时检测按键是否按下 ==================== */
uint8_t Key_IsPressed(Key_ID_t id)
{
    if (id >= KEY_NUM_MAX) return 0;
    return Key_ReadPin(id);
}
