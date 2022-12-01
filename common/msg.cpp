#include "msg.hpp"

#include "utils.hpp"
#include "string.h"

//------------------------------------------------------------------------------
/* Raw */
//------------------------------------------------------------------------------
void initRawMsg(msg_raw_t *msg)
{
    msg->type = message_t::Raw;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}


//------------------------------------------------------------------------------
/* Info & ACK */
//------------------------------------------------------------------------------
void initInfoMsg(struct msg_info_t *msg)
{
    msg->type = message_t::Info;
    msg->timestamp = getTimeStamp();
}
void initAckMsg(msg_ack_t *msg)
{
    msg->type = message_t::Ack;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}


//------------------------------------------------------------------------------
/* Start, Stop, Pause & Resume */
//------------------------------------------------------------------------------
void initStartMsg(struct msg_ack_t *msg)
{
    msg->type = message_t::Start;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}

void initStartMsg(struct msg_mask_t *msg)
{
    msg->type = message_t::Start;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}

void initStopMsg(struct msg_info_t *msg)
{
    msg->type = message_t::Stop;
    msg->timestamp = getTimeStamp();
}

void initPauseMsg(struct msg_info_t *msg)
{
    msg->type = message_t::Pause;
    msg->timestamp = getTimeStamp();
}

void initResumeMsg(struct msg_info_t *msg)
{
    msg->type = message_t::Resume;
    msg->timestamp = getTimeStamp();
}


//------------------------------------------------------------------------------
/* Register & Unregister */
//------------------------------------------------------------------------------
void initRegistrationMsg(msg_registration_t *msg)
{
    msg->type = message_t::Registration;
    msg->timestamp = getTimeStamp();
}

void initUnRegistrationMsg(msg_info_t *msg)
{
    msg->type = message_t::Unregistration;
    msg->timestamp = getTimeStamp();
}




//------------------------------------------------------------------------------
/* Order to free core! */
//------------------------------------------------------------------------------
void initReleaseCoreMsg(msg_empty_t *msg)
{
    msg->type = message_t::ReleaseCore;
    ONLY_DBG( msg->timestamp = getTimeStamp() );    
}


void initCoreFreeMsg(msg_int_t *msg)
{
    msg->type = message_t::CoreFree;
    msg->timestamp = getTimeStamp();
}

void initMaskReducedMsg(struct msg_mask_t *msg)
{
    msg->type = message_t::MaskReduced;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}




//------------------------------------------------------------------------------
/* Specific to the application */
//------------------------------------------------------------------------------
void initnTasksUpdateMsg(struct msg_task_stats_t *msg)
{
    msg->type = message_t::UpdateNTasks;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}

void initAskNewCpuMaskMsg(struct msg_ack_t *msg)
{
    msg->type = message_t::Ask_newCpumask;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}


void initCpuMaskMsg(struct msg_mask_t *msg)
{
    msg->type = message_t::CpuMask;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}

// void initCpuMaskAppliedMsg(struct msg_mask_t *msg)
// {
//     msg->type = message_t::CpuMaskApplied;
//     ONLY_DBG( msg->timestamp = getTimeStamp() );
// }

// void initCoreFreeMsg(struct msg_mask_t *msg)
// {
//     msg->type = message_t::CoreFree;
//     ONLY_DBG( msg->timestamp = getTimeStamp() );
// }
    
// void initCoreOccupied(struct msg_mask_t *msg)
// {
//     msg->type = message_t::CoreOccupied;
//     ONLY_DBG( msg->timestamp = getTimeStamp() );
// }



//------------------------------------------------------------------------------
/* Related with Power Budget */
//------------------------------------------------------------------------------
void initAskNewBudgetMsg(struct msg_float2_t *msg)
{
    msg->type = message_t::AskNewBudget;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}

void initAssignBudgetMsg(struct msg_float_t *msg)
{
    msg->type = message_t::AssignBudget;
    ONLY_DBG( msg->timestamp = getTimeStamp() );
}


//------------------------------------------------------------------------------
/* Utils */
//------------------------------------------------------------------------------
void copyMaskToMsg(dynamic_bitset mask, struct msg_mask_t *msg)
{
    int len;
    char* p = mask.serialize(&len);

    

    
    for(int i=0; i<len; i++)
	msg->data[i] = p[i];

    delete[] p;
}

dynamic_bitset getMaskFromMsg(struct msg_mask_t *msg)
{
    return dynamic_bitset(msg->data);
}






int msgType_toStr(char* str, message_t msgType, int strSize)
{
    char str_internal[80]="";
    
    switch(msgType){
    case message_t::Raw:             
	strcpy(str_internal, "Raw"); break;
    case message_t::Info:            
	strcpy(str_internal, "Info"); break;
    case message_t::Ack:
	strcpy(str_internal, "Ack"); break;
    case message_t::Start:
	strcpy(str_internal, "Start"); break;
    case message_t::Stop:
	strcpy(str_internal, "Stop"); break;
    case message_t::Pause:
	strcpy(str_internal, "Pause"); break;
    case message_t::Resume:
	strcpy(str_internal, "Resume"); break;

    case message_t::Registration:
	strcpy(str_internal, "Registration"); break;
    case message_t::Unregistration:
	strcpy(str_internal, "Unregistration"); break;

    case message_t::ReleaseCore:
	strcpy(str_internal, "ReleaseCore"); break;
    case message_t::CoreFree:
	strcpy(str_internal, "CoreFree"); break;
    case message_t::MaskReduced:
	strcpy(str_internal, "MaskReduced"); break;

    case message_t::Ask_newCpumask:
	strcpy(str_internal, "ask_newCpuMask"); break;
    case message_t::CpuMask:
	strcpy(str_internal, "cpuMask"); break;
	
    case message_t::UpdateNTasks:
	strcpy(str_internal, "nTasksUpdate"); break;
    case message_t::CpuMaskApplied:
	strcpy(str_internal, "cpuMaskApplied"); break;
    case message_t::CoreOccupied:
	strcpy(str_internal, "coreOccupied"); break;

    case message_t::AskNewBudget:
	strcpy(str_internal, "askNewBudget"); break;
    case message_t::AssignBudget:
	strcpy(str_internal, "assignBudget"); break;
    }

    
    strncpy(str, str_internal, strSize);
    return strlen(str);    
}

