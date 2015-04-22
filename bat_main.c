#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "osal.h"
#include "bat_man.h"
#include "volt_meas.h"
#include "lcd_disp.h"
#include "gprs.h"

typedef enum{
    LEVEL_ERROR = 3U,
    LEVEL_WARN = 4U,
    LEVEL_INFO = 6U,
    LEVEL_DEBUG = 7U,
}OS_LogLevel_t;

static void logHex(uint8_t *buf, uint32_t bufsize, uint8_t *ptr, uint32_t length)
{
    uint32_t hexLen = 0;
    uint32_t i = 0;
    uint32_t len = 0;

    hexLen = (bufsize > length) ? length : bufsize;
    if(bufsize > 0){
        while(i < hexLen && len < bufsize){
            sprintf((char *)(buf + len), "%02x ", ptr[i]);
            i++;
            len += 3;
        }
    }
}

void osLog(OS_LogLevel_t level, uint8_t *ptr, uint32_t len, char *format, ...)
{
    va_list ap;
    uint8_t buffer[2048];
    uint32_t strLen = 0;

    memset(buffer, 0, sizeof(buffer));

    va_start(ap, format);
    vsprintf((char *)buffer, format, ap);
    va_end(ap);

    strLen = strlen((char *)buffer) > 2048 ? 2048 : strlen((char *)buffer);

    if(strLen < 2048 && len > 0){
        logHex(buffer + strLen, 2048 -strLen, ptr, len);
    }

    openlog("/Bat.log", LOG_ODELAY, LOG_USER); /////
    syslog(level, "%s", buffer);
    closelog();
}
uint8_t logLevel = 8;

void ba_log(OS_LogLevel_t level, const char * func, uint8_t *ptr, uint32_t len, const char *fmt, ...)
{
#if 1
    char buf[2048];
    va_list ap;
    va_start(ap, fmt);

    vsnprintf(buf, sizeof(buf), fmt, ap);

    if(0 != strlen(func)){
        osLog(level, ptr, len, "%s: %s", func, buf);
    }else{
        osLog(level, ptr, len, "%s", buf);
    }

    va_end(ap);
#endif
}
#define ba_log_info(...)  do{ if(logLevel >= LEVEL_INFO){ba_log(LEVEL_INFO, "", __VA_ARGS__);}}while(0)

#define MAX_CONF_BUF_LEN   (128U)
#define CONFIG_FILE_PATH   "/bat/bat.conf"

void ba_load_config(void)
{
    FILE* conf;
    char str[MAX_CONF_BUF_LEN];

    memset(str, 0, MAX_CONF_BUF_LEN);
    conf = fopen(CONFIG_FILE_PATH, "r");
    if(conf == NULL){
        printf("can not find config file\n");
    }else{
        
        fclose(conf);
    }
}

void ba_handle_event(){}

uint32_t g_tick_cnt = 0U;

void ba_tick_timer_handler(void)
{
    struct os_msg_t * p_msg;
    g_tick_cnt++;

    printf("Tick timer count = %d\n", g_tick_cnt);

    if(0U == (g_tick_cnt % 10U)){
        if(os_alloc_msg(&p_msg)){
            p_msg->evt_code = MSG_VOLT_MEAS;
            if(0 == os_send_msg(TASK_VOLT_MEAS, p_msg)){
                printf("sending msg\n");
            }
        }else{
            printf("alloc msg fails\n");
        }
    }

    if(0U == (g_tick_cnt % 60U)){
//        if(os_alloc_msg(&p_msg)){
//            p_msg->evt_code = MSG_THERM_MEAS;
//            os_send_msg(TASK_THERM_MEAS, p_msg);
//        }
    }

    if(0U == (g_tick_cnt % 3600U)){
//        if(os_alloc_msg(&p_msg)){
//            p_msg->evt_code = MSG_WRITE_REC;
//            //  p_msg->data can be used to carry additional paras
//            os_send_msg(TASK_FLASH_MANA, p_msg);
//        }
    }

}
#define BA_VERSION "R1.03.02"
int main(int argc, void *argv[])
{
    ba_log_info(NULL, 0, "  Battery Monitor Version : %s", BA_VERSION);

    ba_load_config();

    os_init_tick_timer(&ba_tick_timer_handler);
    os_init_semph();

    os_init_msg_list();
    os_create_task((void *)lcd_disp_task, 1024 * 1024);
//  os_create_task((void *)thermal_task, 1024 * 1024);
    os_create_task((void *)gprs_task, 1024 * 1024);
    os_create_task((void *)volt_meas_task, 1024 * 1024);

    os_post_semph(SEMPH_MSSG);
    printf("This is main thread, loop forever!\n");
    while(1){
        ba_handle_event();
    }
    
    return 0;
}
