#include <stdio.h>
#include "osal.h"
#include "bat_man.h"

void lcd_disp_task(void)
{
    struct os_msg_t *p_msg = NULL;

    printf("lcd display task is running.\n");


    while(1){
        os_recv_msg(TASK_LCD_DISP, &p_msg, TO_WAIT_FOREVER);
        switch(p_msg->evt_code){
            case MSG_LCD_DISP:
                //// make new content for LCD and refresh it
                os_free_msg(p_msg);
                break;
            case 2:
                break;
            default:
                break;
        }
    }
}   
