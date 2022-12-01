
#include "client.h"
#include "GlobalState.hpp"

#include <sys/types.h>
#include <unistd.h>

#include "common/msg.hpp"
#include "common/utils.hpp"



int SS_initLibrary( void )
{
#ifdef DEBUG
    fprintf(stderr, "===================================\n");
    fprintf(stderr, "= ProcessId:    %d\n", getpid());
    fprintf(stderr, "= CreationTime: %llu\n", getTimeStamp());
    fprintf(stderr, "===================================\n");
#endif
    int retvalue = 0;

    GlobalState::comm = new CommunicationManager();
    if (GlobalState::comm == NULL) retvalue=-1;

    GlobalState::id = getpid();

    if(GlobalState::comm->registerClient( GlobalState::id )<0 || retvalue != 0)
	return -1;
    else
	return GlobalState::id;
}



int SS_finishLibrary(void)
{
    ONLY_DBG( puts("Finish Library") );
    int retval=0;
    
    retval=GlobalState::comm->unregisterClient();
    delete GlobalState::comm;

    return retval;
}


int SS_startExecution(dynamic_bitset *bt)
{
    int ret;
    //A) Send start request
    msg_ack_t msg_ack;
    initStartMsg(&msg_ack);

    if((ret=GlobalState::comm->sendMsg(sizeof(struct msg_ack_t), &msg_ack, true)) < 0){
      return ret;
    }
      

    //B) Ask for the mask? Wait for it?
    msg_mask_t msg;
    bt->clear();
    
    if((ret=GlobalState::comm->rcvMsg(&msg, message_t::Start, true)) < 0) //blocks
	return -1;

    *bt = getMaskFromMsg(&msg);
    
    //Maybe, we have received a new cpuMask right after the start message:
    msg_mask_t msg2;
    while((ret=GlobalState::comm->rcvMsg(&msg2, message_t::CpuMask, false))>0){
	*bt = getMaskFromMsg(&msg2);
    }

    while(bt->count() == 0){
      GlobalState::comm->rcvMsg(&msg2, message_t::CpuMask, true);
      *bt = getMaskFromMsg(&msg2);
    }
    
    return 0;
}


int SS_needToFreeCore()
{
    msg_empty_t msg;
    return GlobalState::comm->rcvMsg(&msg, message_t::ReleaseCore, false);    
}

int SS_informFreeCore(int coreId, dynamic_bitset *newMask)
{
    ONLY_DBG( puts("Inform Free Core") );
    msg_int_t msg;
    initCoreFreeMsg(&msg);
    msg.data=coreId;

    
    int ret;
    if((ret=GlobalState::comm->sendMsg(sizeof(msg), &msg))<0){
	return ret;
    }


    return 0;

    //YA NO ESPRAMOS A LA RESPUESTA. CONSUMIR MSG??
    // msg_mask_t msg2;
    // ret = GlobalState::comm->rcvMsg(&msg2, message_t::MaskReduced, true); //block here
    // *newMask = getMaskFromMsg(&msg2);
    
    //return ret;
}


int SS_updateNtasks(int nCrit, int nNotCrit)
{
    msg_task_stats_t msg;
    initnTasksUpdateMsg(&msg);

    msg.nCrit = nCrit;
    msg.nNotCrit = nNotCrit;

    return GlobalState::comm->sendMsg(sizeof(struct msg_task_stats_t), &msg);
}


int SS_askForNewCpuMask(dynamic_bitset *bt)
{
    /*** Chapuzas para compatibilidad con lo anterior:
     * a) Consumimos todos los mensajes que puede haber en el camino
     * b) Preguntamos y que nos respondan en ese momento
     * 
     * La otra opcion es consumir todos los mensajes que existan y quedarnos 
     * con el último. en caso de no haber ninguno, bloquear hasta que haya alguno
     *
     * Implementada la opcion II
     **/

    int ret; 
    msg_mask_t msg;
    
    bt->clear();
    
    while((ret=GlobalState::comm->rcvMsg(&msg, message_t::CpuMask, false)) > 0){
	*bt = getMaskFromMsg(&msg);
    }

    if(bt->count() !=0) //Cores received :)
	return 0;
    
    
    if((ret=GlobalState::comm->rcvMsg(&msg, message_t::CpuMask, true)) > 0){
	*bt = getMaskFromMsg(&msg);
    }
    return ret;
    
    // if((ret=GlobalState::comm->rcvMsg(&msg2, message_t::CpuMask, true))<0)
    
    // while(bt->count()==0){
    // 	msg_ack_t msg;
    // 	initAskNewCpuMaskMsg(&msg);

    // 	if((ret=GlobalState::comm->sendMsg(sizeof(struct msg_ack_t), &msg)) < 0)
    // 	    return ret;

    // 	msg_mask_t msg2;
    // 	if((ret=GlobalState::comm->rcvMsg(&msg2, message_t::CpuMask, true))<0)
    // 	    return ret;

    // 	*bt = getMaskFromMsg(&msg2);
    // }

    // return 1;
}



int SS_askForNewCpuMask_noBlock(dynamic_bitset *bt)
{
  int ret; 
  msg_mask_t msg;
    
  bt->clear();
    
  while((ret=GlobalState::comm->rcvMsg(&msg, message_t::CpuMask, false)) > 0){
    *bt = getMaskFromMsg(&msg);
  }

  if(bt->count() !=0) //Cores received :)
    return 0;
  else
    return -1;
}

// int SS_confirmMaskApplied(dynamic_bitset *bt)
// {
//     msg_mask_t msg;
//     initCpuMaskAppliedMsg(&msg);

//     copyMaskToMsg(*bt, &msg);

//     return GlobalState::comm->sendMsg(sizeof(struct msg_mask_t), &msg);
// }


// int SS_confirmCoreFree(int nCore)
// {
//     dynamic_bitset bt;
//     bt.set(nCore);

//     msg_mask_t msg;
//     initCoreFreeMsg(&msg);

//     copyMaskToMsg(bt, &msg);
//     return GlobalState::comm->sendMsg(sizeof(struct msg_mask_t), &msg, false);
// }

// int SS_confirmCoresFree(dynamic_bitset *bt)
// {
//     msg_mask_t msg;
//     initCoreFreeMsg(&msg);

//     copyMaskToMsg(*bt, &msg);
//     return GlobalState::comm->sendMsg(sizeof(struct msg_mask_t), &msg, false);
// }



// int SS_confirmCoreOccupied(int nCore)
// {
//     dynamic_bitset bt;
//     bt.set(nCore);

//     msg_mask_t msg;
//     initCoreOccupied(&msg);

//     copyMaskToMsg(bt, &msg);
//     return GlobalState::comm->sendMsg(sizeof(struct msg_mask_t), &msg, false);
// }


// int SS_confirmCoresOccupied(dynamic_bitset *bt)
// {
//     msg_mask_t msg;
//     initCoreOccupied(&msg);

//     copyMaskToMsg(*bt, &msg);
//     return GlobalState::comm->sendMsg(sizeof(struct msg_mask_t), &msg, false);
// }




float SS_requestNewBudget(float currentBudget, float desiredBudget)
{

  msg_float2_t msg1;
    initAskNewBudgetMsg(&msg1);
    msg1.data[0] = currentBudget;
    msg1.data[1] = desiredBudget;

    //SEND
    int ret;
    if((ret=GlobalState::comm->sendMsg(sizeof(struct msg_float2_t), &msg1, true)) < 0){
	return ret;
    }


    //RECEIVE
    msg_float_t msg2;
    if((ret=GlobalState::comm->rcvMsg(&msg2, message_t::AssignBudget, true))<0)	{
    	return ret;
    } else {
    	return msg2.data;
    }
}
