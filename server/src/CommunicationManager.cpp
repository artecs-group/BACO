#include "CommunicationManager.hpp"

#include "common/utils.hpp"
#include "common/msg.hpp"

#include "ExtraeInstrumentation.hpp"
#include "LogHelper.hpp"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <cstdlib> //NULL definition
#include <cstring>
#ifdef DEBUG
#include <errno.h>
#include <cstdio>
#include <cassert>
#include <signal.h>
#endif

CommunicationManager::CommunicationManager(int key_id, const char *key_path) : _fd(-1)
{
    key_t key_msg;

    //A Get the queue key
    if((key_msg=ftok(key_path, key_id))==-1){
	Log::WARNING_MSG(__FILE__ ": Error getting the queue key");
	DIE(__FILE__ ": Error getting the queue key");
    }
    //B) Get the queue fd
    if((_fd=msgget(key_msg, 0600|IPC_CREAT))==-1){
	Log::WARNING_MSG(__FILE__ ": Error getting the fd");
	DIE(__FILE__ ": Error getting the queue fd");
    }

    ONLY_DBG(_clientId = key_id);
}


CommunicationManager::~CommunicationManager()
{
    //NO borramos la cola directamente, cada uno es responsable de borrar la suya
  
    // if(_fd != -1)
    //   this->close();
}


int CommunicationManager::close()
{
    // if(_fd !=-1 ){
    // 	if(msgctl(_fd, IPC_RMID, (struct msqid_ds *)NULL) == -1){
    // 	    _fd = -1;
    // 	    return -1;
    // 	}else
    // 	    _fd = -1;
    // }
    _fd = -1;
  
    return 0;
}



int CommunicationManager::getMsg(void *msg, message_t msgType, bool wait)
{
    int size, msgflgs;
    switch(msgType){
    case message_t::Raw:
	size = sizeof(struct msg_raw_t); break;

    case message_t::Info:
	size = sizeof(struct msg_info_t); break;	
    case message_t::Ack:
	size = sizeof(struct msg_ack_t); break;

    case message_t::Start:
	size = sizeof(struct msg_mask_t); break;
    case message_t::Stop:
    case message_t::Pause:
    case message_t::Resume:
	size = sizeof(msg_empty_t); break;

    case message_t::Registration:
	size = sizeof(struct msg_registration_t); break;
    case message_t::Unregistration:
	size = sizeof(struct msg_info_t); break;

    case message_t::ReleaseCore:
	size = sizeof(msg_empty_t); break;
    case message_t::CoreFree:
	size = sizeof(msg_int_t); break;
    case message_t::MaskReduced:
	size = sizeof(struct msg_mask_t); break;
	 
    case message_t::Ask_newCpumask:
	size = sizeof(msg_empty_t); break;
    case message_t::CpuMask:
	size = sizeof(msg_mask_t); break;

    case message_t::UpdateNTasks:
	size = sizeof(struct msg_task_stats_t); break;
    case message_t::CpuMaskApplied:	
	size = sizeof(struct msg_ack_t); break;

    case message_t::AskNewBudget:
	size = sizeof(struct msg_float2_t); break;
    case message_t::AssignBudget:
	size = sizeof(struct msg_float_t); break;
	
    default:
	Log::WARNING_MSG(__FILE__ "No se ha implementado todavía este tipo de mensaje");
	DIE(__FILE__ "No se ha implementado todavía este tipo de mensaje");
	size = -1;
	break;
    }

    msgflgs = (wait) ? 0 : IPC_NOWAIT;
    int retval=msgrcv(_fd, msg, size - sizeof(long), msgType, msgflgs);

#ifdef DEBUG
    if(retval==-1){
	char err[20];
      
	switch(errno){
	case E2BIG : strcpy(err,"E2BIG");  break;
	case EACCES: strcpy(err,"EACCES"); break;
	case EFAULT: strcpy(err,"EFAULT"); break;
	case EIDRM : strcpy(err,"EIDRM");  break;
	case EINTR : strcpy(err,"EINTR");  break;
	case EINVAL: strcpy(err,"EINVAL - getMsg"); break;
	case ENOMSG: strcpy(err,"ENOMSG"); break;
	case ENOSYS: strcpy(err,"ENOSYS"); break;
        }

        char str[80];
        sprintf(str, "ERROR [%s] al recibir el mensaje de tipo: %d, con size: %d y fd: %d y cliente: %d\n", err, msgType, size, _fd, _clientId);
	Log::WARNING_MSG(str);
	DIE("Error al recibir el mensaje");
	abort();
    }  


    if(retval >= 0){
	unsigned long long t = getTimeStamp();
	char str[20];
	msgType_toStr(str, (enum message_t)((msg_raw_t*)msg)->type, 20);

	char txt[100];
	sprintf(txt, "(T: %lu-%s)[%d] snd:%llu, rcv:%llu, diff:%llu\n",
		((msg_raw_t*)msg)->type, str, _clientId,
		((msg_raw_t*)msg)->timestamp,t, t-((msg_raw_t*)msg)->timestamp);
	Log::DEBUG_MSG(txt);
    }
#endif

    
#ifdef INSTR
    if(retval>=0)
	ExtraeInstrumentation::Singleton()->emitEvent("Msg-rcv", 1);
#endif
    return retval;
}


int CommunicationManager::sendMsg(int size, void *msg, bool wait)
{
    int msgflags = (wait) ? 0 : IPC_NOWAIT;
  
#ifdef DEBUG
    assert(_fd != -1);
    assert(size >= (int)sizeof(long));
#endif
  
    int retval = msgsnd(_fd, msg, size - sizeof(long), msgflags);

    
#ifdef DEBUG
    if(retval==-1){
	char err[20];
      
	switch(errno){
	case EACCES: strcpy(err,"EACCESS"); break;
	case EAGAIN: strcpy(err,"EAGAIN"); break;
	case EFAULT: strcpy(err,"EFAULT"); break;
	case EIDRM:  strcpy(err,"EIDRM"); break;
	case EINTR:  strcpy(err,"EINTR"); break;
	case EINVAL: strcpy(err,"EINVAL - sendMsg"); break;
	case ENOMEM: strcpy(err,"ENOMEM"); break;
	}
    }
#endif
    
#ifdef INSTR
    if(retval >=0)
	ExtraeInstrumentation::Singleton()->emitEvent("Msg-snd", 1);
#endif
  
    return retval;
}
