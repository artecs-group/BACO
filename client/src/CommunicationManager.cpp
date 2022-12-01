#include "CommunicationManager.hpp"
#include "GlobalState.hpp"
#include "common/msg.hpp"
#include "common/utils.hpp"

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include <cstdlib> //NULL
#include <errno.h>

#ifdef DEBUG
#include <cstdio>
#include <iostream>
#include <cassert>
#endif

CommunicationManager::CommunicationManager()
{
    _fdServer = -1;
    _fdClientIn  = -1;
    _fdClientOut  = -1;

    key_t key_msg;
    //a) Get the queue key
    if((key_msg=ftok(SYSV_KEY_PATH, SYSV_KEY_NUMBER))==-1){
	DIE(__FILE__ ": Error getting the queue key");
    }
  
    //b) Get the queue fd
    if((_fdServer=msgget(key_msg, 0600))==-1){
	DIE(__FILE__ ": Error getting the queue fd");
    }
}

CommunicationManager::~CommunicationManager()
{
    //Esto solo pasa si no se ha llamado a finalizar
    if(_fdClientIn != -1  || _fdClientOut != -1)
	unregisterClient();
}


int CommunicationManager::registerClient( int id )
{
    int retval;

    //A) Send registration msg to the server
    msg_registration_t msg;
    initRegistrationMsg(&msg);
    msg.id         = id;

    if((retval=this->sendMsg(sizeof(struct msg_registration_t), (void *)&msg, true, true)) < 0)
	return retval;

    // B) Create our own queues (nput/output)
    if((retval=this->createClientQueue()) < 0 )
	return retval;

    msg_ack_t ackmsg;
    ackmsg.type = message_t::Ack;

    //C) Wait for an ACK in our own queue
    if((retval=this->rcvMsg(&ackmsg, message_t::Ack)) < 0)
	return retval;

    return 0;
}


int CommunicationManager::sendMsg(int size, void *msg, bool wait, bool server)
{
    int msgflag = (wait) ? 0 : IPC_NOWAIT;
    int fd = (server) ? _fdServer : _fdClientOut;

    if( fd == -1)
	return -1;
    
  
    int retval = msgsnd(fd, msg, size - sizeof(long), msgflag);

#ifdef DEBUG
    if(retval==-1){
	switch(errno){
	case EACCES: puts("EACCESS"); break;
	case EAGAIN: puts("EAGAIN"); break;
	case EFAULT: puts("EFAULT"); break;
	case EIDRM:  puts("EIDRM"); break;
	case EINTR:  puts("EINTR"); break;
	case EINVAL: puts("EINVAL"); break;
	case ENOMEM: puts("ENOMEM"); break;
	}
    }
#endif

    return retval;
}


int CommunicationManager::rcvMsg(void *msg, message_t msgType, bool wait)
{
    int size;
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
	DIE(__FILE__ "No se ha implementado todavía este tipo de mensaje");
	size = -1;
	break;
    }
    
    int msgflgs = (wait) ? 0 : IPC_NOWAIT;
       
    int retval=msgrcv(_fdClientIn, msg, size - sizeof(long), msgType, msgflgs);

#ifdef DEBUG
    if(retval >= 0){ // && errno == ENOMSG){
	unsigned long long t = getTimeStamp();
	fprintf(stdout, "(T: %ld) snd:%llu, rcv:%llu, diff:%llu\n",
		((msg_raw_t*)msg)->type,((msg_raw_t*)msg)->timestamp, t, t-((msg_raw_t*)msg)->timestamp);
	fflush(stdout);
    }

if(retval==-1){
        switch(errno){
        case EACCES: puts("EACCESS"); break;
        case EAGAIN: puts("EAGAIN"); break;
        case EFAULT: puts("EFAULT"); break;
        case EIDRM:  puts("EIDRM"); break;
        case EINTR:  puts("EINTR"); break;
        case EINVAL: puts("EINVAL"); break;
        case ENOMEM: puts("ENOMEM"); break;
        }
    }

#endif

    
    return retval;
  
}


int CommunicationManager::createClientQueue(int id)
{
    int key_number=(id > 0) ? id : GlobalState::id;
    key_t key_msg;

    //A.- Input queue
    //A.a) Get the queue key
    if((key_msg=ftok(SYSV_KEY_PATH_ClientIN, key_number))==-1){
	DIE(__FILE__ ": Error getting the queue key (client)");
    }
	    
    //A.b) Get the queue fd
    if((_fdClientIn=msgget(key_msg, 0600|IPC_CREAT))==-1){
	DIE(__FILE__ ": Error getting the queue fdIn (client)");
    }

    //B.- Output queue
    //B.a) Get the queue key
    if((key_msg=ftok(SYSV_KEY_PATH_ClientOUT, key_number))==-1){
	DIE(__FILE__ ": Error getting the queue key (client)");
    }
    //B.b) Get the queue fd
    if((_fdClientOut=msgget(key_msg, 0600|IPC_CREAT))==-1){
	DIE(__FILE__ ": Error getting the queue fdIn (client)");
    }

    return 0;
}


int CommunicationManager::closeQueues()
{
    if(_fdClientIn != -1){
	if(msgctl(_fdClientIn, IPC_RMID, (struct msqid_ds *)NULL) == -1){
	    DIE(__FILE__ ": Error closing the [in] queue");
	    return -1;
	}
	_fdClientIn = -1;
    }

    if(_fdClientOut != -1){
	if(msgctl(_fdClientOut, IPC_RMID, (struct msqid_ds *)NULL) == -1){
	    DIE(__FILE__ ": Error closing the [out] queue");
	    return -1;
	}
	_fdClientOut = -1;
    }

    return 0;
}


int CommunicationManager::unregisterClient()
{
    ONLY_DBG(  puts("estoy desregistrandome") );
    int retval;

    //A) Send unregistration msg to the server
    msg_registration_t msg;
    msg.type = message_t::Unregistration;
    msg.id = GlobalState::id;
    msg.timestamp = getTimeStamp();

    if((retval=this->sendMsg(sizeof(struct msg_registration_t), (void *)&msg, false)) < 0){
	ONLY_DBG( std::cout << __FILE__ << " - " << __LINE__ << std::endl );
	return retval;
    }
  
    //B) Close our queue
    _fdClientIn = _fdClientOut = -1;
    // if((retval=this->closeQueues())<0){
    // 	ONLY_DBG( std::cout << __FILE__ << " - " << __LINE__ << std::endl );
    // 	return retval;
    // }

    return 0;
}
