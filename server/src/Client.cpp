#include "Client.hpp"

#include "Server.hpp"
#include "common/msg.hpp"
#include "common/utils.hpp"

#include <cstdio>
#include "Server.hpp"
#include <unistd.h>

#include "LogHelper.hpp"
#ifdef DEBUG
#include <cassert>
#endif


Client::Client(int id, Server *srv) : _id(id), _comIn(nullptr), _comOut(nullptr),
				      _srv(srv), _info(nullptr)
{
    _comOut = new CommunicationManager(id, SYSV_KEY_PATH_ServerOUT);
    _comIn = new CommunicationManager(id, SYSV_KEY_PATH_ServerIN);
}



Client::~Client()
{
    // _comIn->close();
    // _comOut->close();

    delete _comIn;
    delete _comOut;
}



/****** 
	MAIN LOOP 
                  ******/
void Client::Run()
{
    this->startCommunication();
    
    msg_raw_t msg;
    bool end = false;
    
    while(!end && _comIn->getMsg(&msg, message_t::Raw)!=-1){
	/* MENSAJES QUE PUEDE RECIBIR Y TIENE QUE PROCESAR (salvo el de registration,
	   que lo recibe y procesa el Clientsupervisor)
	*/
	switch(msg.type){
	case message_t::Unregistration:
	    this->processMsgUnregister((msg_registration_t*)&msg);
	    end = true;
	    break;
	case message_t::UpdateNTasks:
	    this->processMsgUpdateNTasks((msg_task_stats_t*)&msg);
	    break;
	case message_t::Ask_newCpumask:
	    this->processMsgSendNewMask((msg_ack_t*)&msg);
	    break;
	case message_t::CpuMaskApplied:
	    this->processMsgConfirmMask((msg_mask_t*)&msg);
	    break;
	case message_t::CoreFree:
	    this->processMsgCoreFree((msg_int_t*)&msg);
	    break;
        case message_t::Start:
            this->processMsgStart((msg_ack_t*)&msg);
            break;
	case message_t::AskNewBudget:
	    this->processMsgRequestBudget((msg_float2_t*)&msg);
	    break;
	case message_t::AssignBudget:
	    Log::WARNING_MSG(__FILE__ "AssignBudget no deberia llegarle al servidor...");
	    DIE(__FILE__ "Error logico");
	    break;
	    
	case message_t::CoreOccupied:
 	default:
	    Log::WARNING_MSG(__FILE__ "Msg todavía no implementado");
	    DIE(__FILE__ "Todavía no implementado");
	    break;
	}
    }

    delete this;    
}


void Client::setClientInfo(ClientInfo *clInfo)
{
    _info = clInfo;
}



int Client::startCommunication()
{
    msg_ack_t msg;
    initAckMsg(&msg);

    return _comOut->sendMsg(sizeof(struct msg_ack_t), (void *)&msg);
}


/*============= Input Messages =====================================*/
int Client::processMsgStart( msg_ack_t* msg              __attribute__((unused)))
{
  _srv->processStartRequest(_id);
  return 0;
}


int Client::processMsgUnregister(msg_registration_t *msg)
{
    ONLY_DBG( assert(msg->id == _id) );
    _srv->unregister(_id);

    int retval = _comIn->close();
    if(_comOut->close() == -1)
	return -1;
    else
	return retval;
}


int Client::processMsgUpdateNTasks(msg_task_stats_t *msg)
{
    _srv->updateNtasks(_id, msg->nCrit, msg->nNotCrit);
    return 0;
}


int Client::processMsgSendNewMask(msg_ack_t *msg __attribute__ ((unused)) )
{
    _srv->notifyNewCpuMask(_id);
    return 0;
}


int Client::processMsgConfirmMask(msg_mask_t *msg)
{
    dynamic_bitset bt = getMaskFromMsg(msg);
    _srv->confirmCpuMask(_id, bt);
    return 0;
}


int Client::processMsgCoreFree(msg_int_t *msg)
{
    int coreId = msg->data;
    _srv->confirmCoreFree(_id, coreId);
    return 0;
}


int Client::processMsgRequestBudget(msg_float2_t *msg)
{
    _srv->processMsgRequestBudget(_id, msg->data[0], msg->data[1]);
    return 0;
}
    
/*============= Output Messages =====================================*/

void Client::sendStopMsg() const
{
    msg_info_t msg;
    initStopMsg(&msg);
    this->_comOut->sendMsg(sizeof(struct msg_info_t), &msg, false);

    char str[50];
    sprintf(str, "Sent stop msg to client %d", _id);
    Log::VERBOSE_MSG(str);
}


void Client::startExecution(const dynamic_bitset bt) const
{

    msg_mask_t msg;
    initStartMsg(&msg);
    copyMaskToMsg(bt, &msg);

    _comOut->sendMsg(sizeof(struct msg_mask_t), &msg, false);
    

    char str[200];
    sprintf(str, "Starting client %d with mask %s\n", _id, bt.to_string().c_str());
    Log::VERBOSE_MSG(str);
}


void Client::sendMaskMsg(const dynamic_bitset &bt) const
{
    msg_mask_t msg;
    initCpuMaskMsg(&msg);
    copyMaskToMsg(bt, &msg);

    _comOut->sendMsg(sizeof(struct msg_mask_t), &msg, false);
}


void Client::sendReducedMaskMsg(const dynamic_bitset &bt) const
{
    msg_mask_t msg;
    initMaskReducedMsg(&msg);
    copyMaskToMsg(bt, &msg);
    
    _comOut->sendMsg(sizeof(struct msg_mask_t), &msg, false);
}


void Client::sendReleaseCoreOrder() const
{
    msg_empty_t msg;
    initReleaseCoreMsg(&msg);

    _comOut->sendMsg(sizeof(msg), &msg, false);
}


void Client::sendNewBudget(float budget) const
{
    msg_float_t msg;
    initAssignBudgetMsg(&msg);
    msg.data=budget;

    _comOut->sendMsg(sizeof(msg), &msg, false);
}


