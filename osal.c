#include <errno.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include "osal.h"

bool_t os_init_tick_timer(void * tick_handler)
{
    struct itimerval timer_val;
    sigset_t new_set;
    int res = FALSE;

    memset(&timer_val, 0, sizeof(struct itimerval));

    sigaddset(&new_set, SIGALRM);
    sigfillset(&new_set);
    if(sigprocmask(SIG_UNBLOCK, &new_set, NULL)){
        printf("sig alarm failed\n");
    }else{
        printf("sig alarm success\n");
    }

    timer_val.it_interval.tv_sec = 1;
    timer_val.it_interval.tv_usec = 0;
    timer_val.it_value.tv_sec = 1;
    timer_val.it_value.tv_usec = 0;
    if(setitimer(ITIMER_REAL, &timer_val, NULL)){
        printf("tick timer is not registered\n");
    }else{
        res = TRUE;
        printf("tick timer is registered\n");
    }

    signal(SIGALRM, tick_handler);

    return res;
}

#define CONF_SEMPH_CNT      (5U)

static sem_t semph_array[CONF_SEMPH_CNT];

bool_t os_init_semph(void)
{
    uint8_t i = 0;

    for(i = 0; i < CONF_SEMPH_CNT; i++){
        if(sem_init(&semph_array[i], 0, 1)!= 0){
            return FALSE;
        }
    }

    return TRUE;
}

bool_t os_post_semph(uint32_t sem_id)
{
    sem_post(&semph_array[sem_id - 1]);
    return TRUE;
}

bool_t os_wait_semph(uint32_t sem_id)
{
    sem_wait(&semph_array[sem_id - 1]);
    return TRUE;
}

#define CONF_MSG_CNT   (50U)
#define CONF_TASK_CNT  (5U)

static int sys_msg_que_id[CONF_TASK_CNT];
static uint32_t free_msg_cnt = CONF_MSG_CNT;
static struct os_msg_t mssg_array[CONF_MSG_CNT];
static struct os_msg_t *p_free_msg_head;
void os_init_msg_list(void)
{
    uint32_t i = 0;
    key_t temp;

    memset(mssg_array, 0U, sizeof(mssg_array));
    for(i = 0; i < CONF_MSG_CNT - 1; i++){ // last .next should be left behind
        mssg_array[i].next = &mssg_array[i + 1];
    }
    p_free_msg_head = &mssg_array[0];
    free_msg_cnt = CONF_MSG_CNT;

    temp = ftok("/mnt", 0x17);
    printf("ftok: %d\n", temp); 
    for(i = 0; i < CONF_TASK_CNT; i++){
        sys_msg_que_id[i] = msgget((key_t)(temp + i), IPC_CREAT | 0666);
        printf("que_id[%d] = %d\n", i, sys_msg_que_id[i]);
    }
}

bool_t os_alloc_msg(struct os_msg_t **pp_msg)
{
    bool_t res = FALSE;
    struct os_msg_t *p_msg = NULL;

    os_wait_semph(SEMPH_MSSG);

    if(NULL == p_free_msg_head){
        printf("out of msg!!!!!\n");
    }else{
        p_msg = p_free_msg_head;
        p_free_msg_head = p_free_msg_head->next;
        free_msg_cnt--;

        p_msg->next = NULL;
        p_msg->need_free = TRUE;
        ///memset(p_msg->data, 0U, sizeof(p_msg->data));
        *pp_msg = p_msg;
        res = TRUE;
    }

    os_post_semph(SEMPH_MSSG);

    return res;
}

void os_free_msg(struct os_msg_t *p_msg)
{
    uint32_t cnt = 0;
    uint32_t i = 0;
    uint8_t flag = 0;
    uint32_t seconds;
    uint16_t fraction;

    os_wait_semph(SEMPH_MSSG);

    if(NULL == p_msg){
        //return;
    }else if(p_msg->need_free == FALSE){
        //return;
    }else{
        p_msg->next = p_free_msg_head;
        p_free_msg_head = p_msg;
        memset(p_msg->data, 0U, sizeof(p_msg->data));
        free_msg_cnt++;
    }

    os_post_semph(SEMPH_MSSG);
    printf("free msg cnt = %d\n", free_msg_cnt);
}

bool_t os_send_msg(uint32_t task_id, struct os_msg_t* p_msg)
{
    int res = FALSE;
    struct msg_queue_t msg_que;

    memset(&msg_que, 0U, sizeof(struct msg_queue_t));

    msg_que.msg_type = MSG_PRIO_MIDD;
    msg_que.p_msg = p_msg;
    res = msgsnd(sys_msg_que_id[task_id - 0xA0], &msg_que, \
                  sizeof(struct os_msg_t *), IPC_NOWAIT);

    return res;
}

bool_t os_recv_msg(uint32_t task_id, struct os_msg_t **pp_msg, uint32_t timeout)
{
    int32_t res = -1;
    int32_t msg_flag = 0;
    struct msg_queue_t msg_que;

    memset(&msg_que, 0U, sizeof(struct msg_queue_t));

    if(timeout == TO_NO_WAIT){
        msg_flag = IPC_NOWAIT;
    }

    if(task_id > TASK_INVALID){
        printf("bad return !\n");
        return FALSE;
    }

  //  if(m
    while(1){
        res = msgrcv(sys_msg_que_id[task_id - 0xA0], &msg_que, \
                      sizeof(struct os_msg_t *), 0, msg_flag);
        if(-1 == res){
            if(errno == EINTR){
                continue;
            }else if(msg_flag == IPC_NOWAIT){
                return FALSE;
            }else{
                //
                return FALSE;
            }
        }else{
            *pp_msg = msg_que.p_msg;
            return TRUE;
        }
    }
    return TRUE;
}

bool_t os_create_task(void * thread_func, uint32_t stack_siz)
{
    pthread_t thread;
    pthread_attr_t attr;
    int32_t ret;

    ret = pthread_attr_init(&attr);
    if(ret != 0){
        return FALSE;
    }

    ret = pthread_attr_setstacksize(&attr, stack_siz);
    if(ret != 0){
        return FALSE;
    }

    ret = pthread_create(&thread, &attr, thread_func, NULL);
}

