#ifndef _MSG_HPP_
#define _MSG_HPP_

#include "utils.hpp"
#include "dynamic_bitset.hpp"

#define SYSV_KEY_PATH "/bin/netstat"

#define SYSV_KEY_PATH_ClientIN  "/bin/cat"
#define SYSV_KEY_PATH_ClientOUT "/bin/nano"
#define SYSV_KEY_PATH_ServerIN  SYSV_KEY_PATH_ClientOUT
#define SYSV_KEY_PATH_ServerOUT SYSV_KEY_PATH_ClientIN

#define SYSV_KEY_NUMBER 1
#define MAX_MSG_SIZE 8000
#define MAX_MASK_SIZE 100


enum message_t {
    Raw = 0,         /* 0 */

    Info,            /* 1 */
    Ack,

    Start,           /* 3 */
    Stop,
    Pause,           
    Resume,

    Registration,    /* 7 */  //c->s
    Unregistration,           //c->s

    ReleaseCore,     /* 9 */  //s->c
    CoreFree,                 //c->s
    MaskReduced,              //s->c


    Ask_newCpumask, /* 12 */  //c->s //for retro-compatibility
    CpuMask,                  //s->c
    
    UpdateNTasks,   /* 14 */  //c->s
    CpuMaskApplied,           //c->s (ack)
    CoreOccupied,             //c->s ???

    AskNewBudget,   /* 17 */  //c->s
    AssignBudget,             //s->c
};



/* Generic messages */
struct msg_raw_t {
    long type;
    ONLY_DBG(unsigned long long timestamp)

    char data[MAX_MSG_SIZE];
};

struct msg_ack_t {
    long type;
    ONLY_DBG(unsigned long long timestamp)
    /** WARNING: DO NOT REMOVE THIS FIELD.
        You need it to know if you recieved a message (copied 0 bytes)
        or not (copied 0 bytes).
        That's all becasue sizeof(type) dows not count on the rcvMsg function
    **/
    char dummy;
  
};

struct msg_info_t {
    long type;
    unsigned long long timestamp;

    int data;
};

struct msg_mask_t {
    long type;
    ONLY_DBG(unsigned long long timestamp)

    char data[MAX_MASK_SIZE];
};


/* Specific messages */
struct msg_task_stats_t {
    long type;
    ONLY_DBG(unsigned long long timestamp)

    int nCrit;
    int nNotCrit;
};

struct msg_registration_t {
    long type;
    unsigned long long timestamp;

    int id;
    int nThreads;
};

struct msg_float_t 
{
    long type;
    ONLY_DBG(unsigned long long timestamp)

    float data;
};

struct msg_float2_t
{
    long type;
    ONLY_DBG(unsigned long long timestamp)

    float data[2];
};


    
//typedef msg_raw_t msg1;
typedef msg_ack_t msg_empty_t;
typedef msg_info_t msg_int_t;
//typedef msg_mask_t ;
//typedef msg_task_stats_t asdfasdf;
//typedef msg_registration_t asdf;


void initRawMsg(struct msg_raw_t *msg);

void initInfoMsg(struct msg_info_t *msg);
void initAckMsg(struct msg_ack_t *msg);

void initStopMsg(struct msg_info_t *msg);
void initStartMsg(struct msg_ack_t *msg);
void initStartMsg(struct msg_mask_t *msg);
void initPauseMsg(struct msg_info_t *msg);
void initResumeMsg(struct msg_info_t *msg);

void initRegistrationMsg(struct msg_registration_t *msg);
void initUnRegistrationMsg(struct msg_info_t *msg);

void initReleaseCoreMsg(msg_empty_t *msg);
void initCoreFreeMsg(msg_int_t *msg);
void initMaskReducedMsg(struct msg_mask_t *msg);

			

/* Specific to the application */
void initnTasksUpdateMsg(struct msg_task_stats_t *msg);
void initAskNewCpuMaskMsg(struct msg_ack_t *msg);
void initCpuMaskMsg(struct msg_mask_t *msg);
// void initCpuMaskAppliedMsg(struct msg_mask_t *msg);
// void initCoreFreeMsg(struct msg_mask_t *msg);
// void initCoreOccupied(struct msg_mask_t *msg);

/* Related with powerBudget */
void initAskNewBudgetMsg(struct msg_float2_t *msg);
void initAssignBudgetMsg(struct msg_float_t *msg);


/* utils */
void copyMaskToMsg(dynamic_bitset mask, struct msg_mask_t *msg);
dynamic_bitset getMaskFromMsg(struct msg_mask_t *msg);
    
int msgType_toStr(char* str, message_t msgType, int strSize);


#endif //_MSG_HPP_
