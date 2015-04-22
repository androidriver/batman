#ifndef __OSAL_H_
#define __OSAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define bool_t  unsigned char

#define int8_t   signed char
#define uint8_t  unsigned char
#define int16_t  short
#define uint16_t unsigned short
#define int32_t  signed int
#define uint32_t unsigned int
//#define 

#define NULL     ((void *)0)
#define TRUE     (1U)
#define FALSE    (0U)

#define TO_NO_WAIT         (0U)
#define TO_WAIT_FOREVER    (0xFFFF)

#define MAX_DAT_LEN        (0x80)
typedef enum{
    TASK_ID_01 = 0xA0,
    TASK_ID_02 = 0xA1,
    TASK_ID_03 = 0xA2,
    TASK_ID_04 = 0xA3,
    TASK_ID_05 = 0xA4,
    TASK_ID_06 = 0xA5,
    TASK_INVALID
}task_id_t;

typedef enum{
    MSG_TYPE_01 = 0xAA00,
    MSG_TYPE_02,
    MSG_TYPE_03,
    MSG_TYPE_04,
    MSG_TYPE_05,
    MSG_TYPE_06,
    MSG_TYPE_07,
    MSG_TYPE_08,
    MSG_TYPE_MAX
}msg_type_t;

typedef enum{
    SEMPH_MSSG = 0U,
    SEMPH_BUFF,
    SEMPH_MAX
}semph_type_t;

typedef enum{
    MSG_PRIO_HIGH = 1U,
    MSG_PRIO_MIDD = 2U,
    MSG_PRIO_LOW = 3U
}msg_prio_type_t;

struct os_msg_t{
    uint32_t evt_code;
    uint32_t need_free;
    int8_t data[MAX_DAT_LEN];
    void *caller;
    uint32_t time;
    struct os_msg_t *next;
};

struct msg_queue_t{
    long msg_type;
    struct os_msg_t* p_msg;
};

extern bool_t os_init_semph(void);
extern bool_t os_post_semph(uint32_t sem_id);
extern bool_t os_wait_semph(uint32_t sem_id);
extern bool_t os_send_msg(uint32_t task_id, struct os_msg_t *p_msg);
extern bool_t os_create_task(void * thread_func, uint32_t stack_siz);
extern bool_t os_init_tick_timer(void * tick_handler);

 
#ifdef __cplusplus
}
#endif

#endif
