#include "shareCores-Action.hpp"
#include "Server.hpp"
#include "SysInfo.hpp"
#include <algorithm>
#include "LogHelper.hpp"
#include <cstdio>
#include <set>

ShareCoresAction::ShareCoresAction(Server *srv) :
    Action(srv),
    _changeInProgress(false), _futureChange(false),
    _coreStatus(srv->getSysInfo()->getNumCpus(), CoreStatus::IDLE),
    _clientsWaitingCores()
{}

ShareCoresAction::~ShareCoresAction()
{}

const char* ShareCoresAction::getName() const
{
    return "Share Cores - Action";    
}


// --------------
void ShareCoresAction::registerClient(int id)
{
    dynamic_bitset bt(_srv->getSysInfo()->getNumCpus(), false);
    _srv->getClientInfo()->at(id)->set_activeMask(bt);
}

void ShareCoresAction::startClient(int clientId)
{
    recalculate_coreDistribution();
    _srv->startClient(clientId);
}



void ShareCoresAction::unregisterClient(Client *cl     __attribute__ ((unused)),
					ClientInfo *ci )
{
    int clientId = ci->getId();
    auto& m = ci->get_activeMask();

    int coreId=-1;
    while((coreId=m.find_next(coreId)) >= 0){
	freeCore(coreId);
    }

    //Chapuza para ver si así despertamos al último hilo
    m.clear();
    cl->sendMaskMsg(m);
    

    removeClientFromWaitingQueue(clientId);
    assignFreeCores();
    //updateStatus() -> inside assignFreeCores
    recalculate_coreDistribution();
}




// --------------
void ShareCoresAction::updateNTasks(int clientId      __attribute__ ((unused)),
			  int nCrit                   __attribute__ ((unused)),
			  int nNotCrit                __attribute__ ((unused)) )
{
}


void ShareCoresAction::setNewCpuMask(int clientId     __attribute__ ((unused)) )
{}

void ShareCoresAction::confirmCpuMask(int clientId     __attribute__ ((unused)),
			    dynamic_bitset& bt        __attribute__ ((unused)) )
{}


void ShareCoresAction::confirmCoreFree(int clientId, int coreId )
{
    //A) Mark clientMask as idle
    auto ci = _srv->getClientInfo()->at(clientId);
    auto si = _srv->getSysInfo();
    
    auto& m = ci->get_activeMask();


/** What core should we free?
    First we try to free the core coreId, if this core does not belong to
    this client, the first one we'll find on the mask
**/
    int pos;
    if( m.test(coreId) ) pos = coreId;
    else        	 pos = m.find_first();
    
    m.reset(pos);
    ci->set_activeMask(m);

    //B) Send new cpuMask. 
    _srv->sendReducedMaskMsg(clientId, m);
    
    //C) sysInfo mark as idle
    si->setIdleCore(pos);
    
    //D) put coreStatus as Idle
    std::unique_lock<std::mutex> lock(_status_mtx);
    {
	_coreStatus[pos] = CoreStatus::IDLE;
    }
    lock.unlock();
    
    //E) Assign free Cores
    assignFreeCores();
}




/******************************************************************************
 ** Policies to share cores 
 ******************************************************************************/
void ShareCoresAction::recalculate_coreDistribution()
{
    std::unique_lock<std::recursive_mutex> lock(_changeInProgress_mtx);
    {
	if(!_changeInProgress){
	    _changeInProgress = true;
	    _futureChange     = false; //Process all the waiting events at once
	    
	    distributeCores_equal();
	    assignFreeCores();

	} else {
	    _futureChange = true;
	}
    }
    lock.unlock();
}



/** Given N different clients/apps, try to give the same number of cores to each 
    client.
**/
void ShareCoresAction::distributeCores_equal()
{
    std::map<int, ClientInfo*>* ci = _srv->getClientInfo();
    SysInfo*                    si = _srv->getSysInfo();

    int nClients       = ci->size();
    if(nClients == 0) return;
    int coresPerClient = si->getNumCpus() / nClients;
    int coresRemaining = si->getNumCpus() % nClients;


    Log::DEBUG_MSG("INIT SHARE CORES.......................................................");
    

    std::lock_guard<std::mutex> lock(_status_mtx);
    {
	char txt[100];
	
	for(auto &it : *ci){
	    auto coresIncrement = coresPerClient - it.second->get_activeMask().count();

	    if(coresRemaining>0){
		coresIncrement++;
		coresRemaining--;
	    }

	    if(coresIncrement < 0){
		orderToFreeCores(it.first, -1*coresIncrement);


		sprintf(txt, "ClientId: %d. Removing %d cores\n", it.first, coresIncrement);
		Log::DEBUG_MSG(txt);
	    }	
	    else{
		_clientsWaitingCores.push(std::make_pair(coresIncrement, it.first));

		sprintf(txt, "ClientId: %d. Waiting for %d cores\n", it.first, coresIncrement);
		Log::DEBUG_MSG(txt);
	    }
	
	}
    } // end LOCK

    Log::DEBUG_MSG("END SHARE CORES.......................................................");
}


void ShareCoresAction::orderToFreeCores(int clientId, int nCores)
{
    _srv->sendNewCpuMask(clientId);
  
    for(int i=0; i<nCores; i++){
	_srv->sendReleaseCoreOrder(clientId);
    }
}



void ShareCoresAction::assignFreeCores()
{
    auto cis=_srv->getClientInfo();
    auto sys=_srv->getSysInfo();

    std::set<int> toNotify;
    
    /** WARNING: we can yield the core index between clients to speed up
        the whole process, or interchange the while and for loops **/
    std::lock_guard<std::mutex> lock(_status_mtx);
    {
	while( !_clientsWaitingCores.empty() && this->getNumIdleCores() > 0) {

	    int coresToAssign, clientId;
	    std::tie(coresToAssign, clientId) = _clientsWaitingCores.top();
	    _clientsWaitingCores.pop();

	    for(int i=0; i<_coreStatus.size(); i++){
		if(_coreStatus[i]==CoreStatus::IDLE)
		{
		    _coreStatus[i] = CoreStatus::WAITING_TO_RUN;
		    dynamic_bitset mask =cis->at(clientId)->get_activeMask();
		
		    ONLY_DBG( assert( ! mask.test(i)) );
		    mask.set(i);
		    cis->at(clientId)->set_activeMask(mask);
		
		    ONLY_DBG( assert( sys->getIdleMask().test(i) ) );
		    sys->setActiveCore(i);

		    toNotify.insert(clientId);
		    --coresToAssign;
		
		    break;
		}
	    }
	
	    if(coresToAssign>0){
		_clientsWaitingCores.push(std::make_pair(coresToAssign, clientId));
	    }
	}
    }
    
    for(auto it: toNotify){
	// //TEST
	// auto m = cis->at(it)->get_activeMask();
	// m.clear();
	// m.set(4);
	// //m.set(2);
	// cis->at(it)->set_activeMask(m);
	
	_srv->notifyNewCpuMask(it);
    }


    updateStatus();
}


void ShareCoresAction::freeCore(int coreId)
{
    std::lock_guard<std::mutex> lock(_status_mtx);
    {
	_coreStatus[coreId] = IDLE;
	_srv->getSysInfo()->setIdleCore(coreId);
    }
}



inline int ShareCoresAction::getNumIdleCores() const
{
    return std::count_if(_coreStatus.begin(),
			 _coreStatus.end(),
			 [](CoreStatus st) -> bool {return st==CoreStatus::IDLE;}
	);
    
}

inline int ShareCoresAction::getNumWaiting2RunCores() const
{
    return std::count_if(_coreStatus.begin(),
			 _coreStatus.end(),
			 [](CoreStatus st) -> bool {return st==CoreStatus::WAITING_TO_RUN;}
	);
}

void ShareCoresAction::removeClientFromWaitingQueue(int clientId)
{
    std::priority_queue<std::pair<int, int>> otherQueue;

    std::lock_guard<std::mutex> lock(_status_mtx);
    {
	while(! _clientsWaitingCores.empty()){
	    auto p = _clientsWaitingCores.top(); _clientsWaitingCores.pop();

	    if( p.second != clientId )
		otherQueue.push(std::move(p));
	}

	_clientsWaitingCores = std::move(otherQueue);
    } 
}


void ShareCoresAction::updateStatus()
{
    std::lock_guard<std::recursive_mutex> lock(_changeInProgress_mtx);


    if(_changeInProgress)
    {
	//Hemos acabado?
	if(_clientsWaitingCores.size()==0)
	    _changeInProgress=false;
    }

    if(_futureChange && !_changeInProgress)
    {
	// _changeInProgress=true;
	// _futureChange=false;
	recalculate_coreDistribution();
    }
}
 
