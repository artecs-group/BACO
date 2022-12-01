#include "Server.hpp"

#include "common/msg.hpp"
#include "SysInfo.hpp"

#include "LogHelper.hpp"

#include <thread>
#include <cstdio>
#include <unistd.h>

#include <algorithm>

#ifdef INSTR
#include "ExtraeInstrumentation.hpp"
#endif

#ifdef DEBUG
#include <cassert>
#endif


////////////////////
// PUBLIC methods //
////////////////////


/* Ctrs & dtrs */

Server::Server()
{
    _clSp = new ClientSupervisor(this, SYSV_KEY_NUMBER, SYSV_KEY_PATH);
    _sysInfo = new SysInfo();
}



Server::~Server()
{
    ONLY_DBG( assert(_clients.size()==0 && _clientsInfo.size()==0) );
    
    delete _clSp;
    delete _sysInfo;


    for(auto rit=_actions.rbegin(), rit2=_actions.rend(); rit!=rit2; rit++)
	delete rit->second;
}



/* Methods */
void Server::registerAction(Action *act, int priority)
{
    _actions.push_back(std::make_pair(priority, act));
    std::sort(_actions.begin(), _actions.end());
}


void Server::Run()
{
    std::thread t(&ClientSupervisor::Run, _clSp);  
    t.join();
}


//------------------------------------------------------------------------------
void Server::registerClient(int id, Client* cl)
{
    ONLY_INSTR( ExtraeInstrumentation::Singleton()->emitEvent("client-open", 1) );

    char str[80];
    sprintf(str, "Registered new client with id %d", id);
    Log::INFO_MSG(str);


    /* LOCK */
    std::unique_lock<std::mutex> lock(_clientsMtx);
    
    ClientInfo *clInfo =
	new ClientInfo(id);
    this->insertClient(id, cl, clInfo);
    cl->setClientInfo(clInfo);


    // Call action event
    for(auto it=_actions.begin(), it2=_actions.end(); it!=it2; it++)
	(*it).second->registerClient(id);
    
    lock.unlock();
}


void Server::processStartRequest(int id)
{

  /* LOCK */
  std::unique_lock<std::mutex> lock(_clientsMtx);

  for(const auto &it: _actions)
    it.second->startClient(id);

  //Not needed
  lock.unlock();
}



void Server::unregister(int id)
{
    ONLY_INSTR( ExtraeInstrumentation::Singleton()->emitEvent("client-close", 1) );

    char str[50];
    sprintf(str, "Client %d has gone", id);
    Log::INFO_MSG(str);


    
    /* LOCK */
    std::lock_guard<std::mutex> lock(_clientsMtx);

    //Get client's structures
    Client     *cl = _clients[id];
    ClientInfo *ci = _clientsInfo[id];

    //A) Mark client as stopped
    //ci->setStatus(ClientInfo::status_t::STOPPED);

    //B)
    _clientsInfo.erase(id);    
    _clients.erase(id);
    
    //C) Call action event handler before destroying the structures
    for(auto it=_actions.begin(), it2=_actions.end(); it!=it2; it++)
	(*it).second->unregisterClient(cl, ci);


    //D) delete ClientState instance
    delete ci;

    //E) delete Client instance
    // delete cl; client deletes itself

}

//------------------------------------------------------------------------------
void Server::updateNtasks(int clientId, int nCrit, int nNotCrit)
{
#warning "TODO: implement"
    /* LOCK */
    std::lock_guard<std::mutex> lock(_clientsMtx);

    for(auto it=_actions.begin(), it2=_actions.end(); it!=it2; it++)
	(*it).second->updateNTasks(clientId, nCrit, nNotCrit);
}

void Server::sendNewCpuMask(int clientId)
{
#warning "TODO: implement"
    /* LOCK */
    std::lock_guard<std::mutex> lock(_clientsMtx);

    for(auto it=_actions.begin(), it2=_actions.end(); it!=it2; it++)
	(*it).second->setNewCpuMask(clientId);
}
   
void Server::confirmCpuMask(int clientId, dynamic_bitset &bt)
{
#warning "TODO: implement"
    /* LOCK */
    std::lock_guard<std::mutex> lock(_clientsMtx);

    for(auto it=_actions.begin(), it2=_actions.end(); it!=it2; it++)
	(*it).second->confirmCpuMask(clientId, bt);
}

void Server::confirmCoreFree(int clientId, int coreId)
{
#warning "TODO: implement"
    std::lock_guard<std::mutex> lock(_clientsMtx);

    for(auto& it: _actions)
	it.second->confirmCoreFree(clientId, coreId);
}



void Server::processMsgRequestBudget(int clientId, float currentBudget, float desiredBudget)
{
    std::lock_guard<std::mutex> lock(_clientsMtx);
    for(const auto& it: _actions)
	it.second->requestNewBudget(clientId, currentBudget, desiredBudget);
}


void Server::stop()
{
    /* LOCK */
    std::lock_guard<std::mutex> lock(_clientsMtx);

    //A) Stop the ClientSupervisor in order to not receive more registration messages
    _clSp->stop();


    for(auto it=_clients.begin(), it2=_clients.end(); it!=it2; it++){
	//B) Send stop message to each client
	int id = it->first;
	it->second->sendStopMsg();

	//C) Free memory
	delete it->second;
	delete _clientsInfo[id];
    }
}






/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + Estos métodos no deberían ser invocados directamente, sino como  +
  + resultado de alguna llamada anterior, por lo que se asume que el + 
  + mutex ya esta adquirido y no es necesario volverlo a adquirir    +
  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void Server::startClient(int id)
{
    // dynamic_bitset mask = _clientsInfo[id]->getActiveMask();
    // _clientsInfo[id]->setStatus(ClientInfo::status_t::RUNNING);
    _clients[id]->startExecution( _clientsInfo[id]->get_activeMask() );
}

void Server::notifyNewCpuMask(int id)
{
    _clients[id]->sendMaskMsg(_clientsInfo[id]->get_activeMask());
}

void Server::sendReleaseCoreOrder(int clientId)
{
    _clients[clientId]->sendReleaseCoreOrder();    
}

void Server::sendReducedMaskMsg(int clientId, const dynamic_bitset &bt)
{
    _clients[clientId]->sendReducedMaskMsg(bt);
}


void Server::sendNewBudget(int clientId, float newBudget)
{
    _clients[clientId]->sendNewBudget(newBudget);
}


    

std::map<int, ClientInfo*>* Server::getClientInfo()
{
    return &(this->_clientsInfo);
}


SysInfo* Server::getSysInfo()
{
    return (this->_sysInfo);
}


void Server::insertClient(int id, Client *cl, ClientInfo *clInfo)
{
    _clients.insert(std::pair<int, Client*> (id, cl));
    _clientsInfo.insert(std::pair<int, ClientInfo*> (id, clInfo));
}


