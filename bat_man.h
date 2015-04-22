
#include "osal.h"

typedef enum{
    TASK_VOLT_MEAS = TASK_ID_01,

    TASK_LCD_DISP = TASK_ID_02,
    TASK_THERM_MEAS = TASK_ID_03,
    TASK_FLASH_MANA = TASK_ID_04
}ba_task_id_t;

typedef enum{
    MSG_VOLT_MEAS = MSG_TYPE_01,
    MSG_THERM_MEAS = MSG_TYPE_02,
    MSG_WRITE_REC = MSG_TYPE_03,
    MSG_LCD_DISP = MSG_TYPE_04,
}ba_msg_type_t;

//typedef enum{
//    SEMPH_MSSG = 111,
//    SEMPH_BUFF,
  //  SEMPH_,
//}
