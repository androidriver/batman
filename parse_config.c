#include <stdio.h>
#include <string.h>
#include "osal.h"

#define CONF_ITEM_CNT    (10U)
#define MAX_LINE_BUF_SIZ         (32U)


char confs[CONF_CONF_SECTION_CNT] = {
    {"SERIAL_NUM",   
         "SN",        "",       ""},
    {"BATTERY",
         "count",     "range",  ""},
    {"GPRS",
         "interval",  "",       ""},
    {"RECORD",
         "max_count", "itvl_charge", "itvl_discharge"},
    {"VOLTAGE_3V",
         "low_threshold",   "high_threshold"},
    {"VOLTAGE_5V",
         "low_threshold",  "high_threshold"},
    {"CAPACITY",
         "low_threshold", "high_threshold"},
    {"LCD",
         "refresh_interval" },
    {"WATCHDOG",
         "check_interval"},
    {"TEMPERATURE",
         "low_threshold",   "high_threshold"}
};

uint8_t *get_line(char *buf, uint32_t len, FILE *file)
{
    char *ptr;
    ptr = (char*)fgets((char*)buf, len, file);
    while(ptr != NULL && strlen((char*)ptr) <= 2)
        ptr = fgets((char*)buf, len, file);

    return (uint8_t*)ptr;
}

void trim_space(char *ori, char* trim)
{
    char *ptrsrc;
    char *ptrdst;

    ptrsrc = ori;
    ptrdst = trim;
    while(*ptrsrc != '\0'){
        if((ptrsrc != ' ') && (*ptr != '\r') && (*ptrsrc != '\t') && (*ptrsrc != '\n')){
            *ptrdst = *ptrsrc;
            ptrdes++;
        }
        ptrsrc++;
    }
    ptrdes = '\0';
}

void touch_gConf(uint32_t i, uint32_t j, char *str)
{
    if(j == 0){

    }else if(j == 1){

    }else if(j == 2){

    }else{
        ""
    }
}

bool_t read_config_file(void)
{
    char line_buf[MAX_LINE_BUF_SIZ];
    char buf_trim[MAX_LINE_BUF_SIZ];
    uint32_t i = 0;
    uint32_t j = 0;
    FILE *conf;
    char val_str[32U];

    conf = fopen("/bat/bat.conf", "r");
    if(conf == NULL){
        printf("can not fin /bat/bat.conf!!\n");
        return FALSE;
    }

    do{
        if(FALSE != get_line(line_buf, MAX_LINE_BUF_SIZ, conf)){
            trimspace(line_buf, buf_trim);
            if(buf_trim[0] == '['){
                if(buf_trim[strlen(confs[i].sec_name) + 1] != ']'){
                    return FALSE;
                }

                if(strncmp(confs[i].sec_name, &buf_trim[1], strlen(confs[i].sec_name) != 0)){
                    printf("wrong section name in config file: %s", confs[i].sec_name);
                    return FALSE;
                }

                i++;
                j = 0;
            }else{
                if(strncmp(buf_trim, confs[i].item_name[j], strlen(confs[i].item_name[j]))){
                    read_item_value(buf_trim, val_str);
                    touch_gConf(i, j, buf_trim + strlen(confs[i].item_name[j] + 1U);
                    j++;
                }
            }
        }

    }while(i < CONF_CONF_SECTION_CNT || i < 256U);
}
