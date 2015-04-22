#include <stdio.h>
#include "osal.h"
#include "bat_man.h"


void volt_meas_task(void)
{
    struct os_msg_t *p_msg = NULL;

    printf("This task is used to handle voltage measurement.\n");

    while(1){
        if(TRUE == os_recv_msg(TASK_VOLT_MEAS, &p_msg, TO_WAIT_FOREVER)){
            printf("111111111111\n");
            switch(p_msg->evt_code){
            case MSG_VOLT_MEAS:
                printf("Recv MSG_VOLT_MEAS!\n");
                // gather voltage info here.
                //
                os_free_msg(p_msg);
                break;
            case 2:
                break;
            default:
                break;
            }
        }
    }    
}
