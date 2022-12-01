#include "ClientSupervisor.hpp"

#include "Server.hpp"
#include "Client.hpp"
#include "common/utils.hpp"
#include "LogHelper.hpp"
#include <thread>

ClientSupervisor::ClientSupervisor(Server *srv, int key_id, const char* key_path)
{
    _srv = srv;
    _com = new CommunicationManager(key_id, key_path);
}


ClientSupervisor::~ClientSupervisor()
{
    _com->close();
    delete _com;
}


void ClientSupervisor::Run()
{
    int retval;
    struct msg_raw_t msg;
    initRawMsg(&msg);
    bool end = false;

  
    while(!end && (retval=_com->getMsg(&msg, message_t::Raw))>=0){
	switch(msg.type){
	case message_t::Registration:
	    this->registerClient((struct msg_registration_t*)&msg);
	    break;
	case message_t::Stop:
	    end=true;
	    break;
	default:
	    char txt[120];
	    char mtype[80];
	    msgType_toStr(mtype, (enum message_t)msg.type, 80);
	    sprintf(txt, "%s: %d - Message type not supported (%s)\n",
		    __FILE__, __LINE__, mtype);
            Log::WARNING_MSG(txt);
	    DIE(txt);
	    break;
	}
    
	initRawMsg(&msg); //Prepare the message for the next lap
    }
  
    if(retval<0 && !end){
	Log::WARNING_MSG(__FILE__ "ERROR registering the client");
	DIE(__FILE__ "Error registering the client");
    }
  
}


/* 
   Puesto que el thread está continuamente bloqueado esperando a recibir
   mensajes, envíamos al thread un mensaje de stop para así conseguir que
   se pare a sí mismo.
*/
void ClientSupervisor::stop()
{
    msg_info_t msg;
    initStopMsg(&msg);

    _com->sendMsg(sizeof(struct msg_info_t), &msg, false);
}



void ClientSupervisor::registerClient(msg_registration_t *msg)
{
    Client *cl = new Client(msg->id, _srv);
    
    _srv->registerClient(msg->id, cl);
    std::thread t(&Client::Run, cl);
    t.detach();
}
