#include "MalleableFreq-Action.hpp"
#include "Server.hpp"
#include "SysInfo.hpp"
#include <algorithm>
#include "LogHelper.hpp"
#include <cstdio>
#include <set>
#include <sstream>
#include <iostream>
#include <cassert>




/****************************************************
 **** ESTA ACCIÓN ASUME QUE:
 **** Todos los cores están asignados a alguna aplicación 
 ****   a) no hay cores en idle
 ****   b) Al desregistrarse no tiene cores asignados
 ****************************************************/

MalleableFreqAction::MalleableFreqAction(Server *srv) :
    Action(srv),
    _budgetMargin{1.4}, _budgetInfo{},
    _NCORES{20} //, _idlePower{1.474423832}
{
    char* txt=getenv("GLOBAL_BUDGET");
    if(txt==NULL){
	Log::INFO_MSG("GLOBAL_BUDGET variable not set. Using default value");
	_globalBudget=125.0;
    } else {	
	_globalBudget=atof(txt);
    }

    
    //_availableBudget=_globalBudget - (_NCORES*_idlePower);
    _availableBudget=_globalBudget;
    
#ifdef DEBUG
    std::stringstream ss("");
    ss << "GLOBAL BUDGET: " << _globalBudget << '\n';
    Log::DEBUG_MSG(ss.str());
#endif
}


MalleableFreqAction::~MalleableFreqAction()
{}


const char* MalleableFreqAction::getName() const {
    return "Malleable Freq - Action";    
}


// --------------
void MalleableFreqAction::registerClient(int id)
{
    //_availableBudget += (_NTHREADS_PER_APP * _idlePower);
    _budgetInfo.insert({id, {}});
}


void MalleableFreqAction::startClient(int clientId)
{
    _srv->startClient(clientId);
}


void MalleableFreqAction::unregisterClient(Client *cl  __attribute__ ((unused)),
					   ClientInfo *ci )
{
    const int clientId = ci->getId();

    ONLY_DBG (assert (ci->get_activeMask().count()==0));
    
    //_availableBudget += ( _budgetInfo[clientId].given - (_NTHREADS_PER_APP*_idlePower));
    _availableBudget += _budgetInfo[clientId].given;
    _budgetInfo.erase(clientId);
}

// --------------
void MalleableFreqAction::giveFirstBudget(int clientId, float currentBudget, float desiredBudget){
  ONLY_DBG(assert(currentBudget==0));


#ifdef DEBUG
  std::stringstream ss ("");
  ss << "[" << clientId << "] -- REPARTO INICIAL --  ["
     << currentBudget << ", " <<  desiredBudget << ", "
     << _availableBudget << "]\n";
  Log::DEBUG_MSG(ss.str());
#endif

  const int nClients = _srv->getClientInfo()->size();


  //si solo hay un cliente, le damos todo lo que pida o tengamos
  if( nClients == 1 ) {
      const float given = std::min (desiredBudget, _availableBudget);
      _availableBudget -= given;
      _budgetInfo[clientId].given=given;

      _srv->sendNewBudget(clientId, given);
  }  
  //no hay suficiente budget para darle todo lo que pide (incluso puede que haya 0)
  else if(_availableBudget < desiredBudget){ 
    /** Calcular cuanto nos tiene que dar cada cliente
     *
     * De forma inicial, cada cliente nos debería dar una proporción de tal
     * forma que al final todos tengamos lo mismo "teoricamente".
     * Realmente el calculo se hace suponiendo que el reparto está hecho de
     * forma equitativa entre clientes, aunque en la práctica pueda surgir
     * desbalanceo
     **/


    
    /**
     * Antes el budget era: Global / (N-1).
     * Ahora el budget es:  Global / N.
     * Lo que hay que quitarles es x= (G/(n-1)) - (G/n) = G/((n-1)*n))
     */
    float perdidaBudget = _globalBudget / (((nClients - 1) * nClients)*1.0);
    if(perdidaBudget > desiredBudget) perdidaBudget = desiredBudget;

    //Vamos a quitar budget de todos los clientes, pero solamente hasta cubrir el
    // budget pedido, asi que puede que haya clientes que salgan beneficiados y
    // otros más perjudicados
    const float repartoJusto = _globalBudget / (nClients * 1.0);
    const float aAsignar = std::min(desiredBudget, repartoJusto);

    
    float asignado=0.0;
    
    for(auto it=_budgetInfo.begin(); it!=_budgetInfo.end() && asignado < aAsignar; ++it){

      if(it->first == clientId) continue;
      
      _changesToCommit.insert({ it->first, {clientId, perdidaBudget} });
      asignado += perdidaBudget;
    }

    _toNotify.insert(clientId);
  } else { //Hay budget disponible suficiente para asignarle
    ONLY_DBG(assert(_availableBudget >= desiredBudget));
    _availableBudget -= desiredBudget;
    _budgetInfo[clientId].given=desiredBudget;


    _srv->sendNewBudget(clientId, desiredBudget);
  }

  
}


void MalleableFreqAction::requestNewBudget(int clientId, float currentBudget, float desiredBudget)
{
    auto& cb = _budgetInfo[clientId];
    cb.used     = currentBudget;
    cb.required = desiredBudget;

    if(currentBudget == 0.0){ //PRIMER REPARTO
      std::cout << "PRIMER REPARTO" << std::endl;
      giveFirstBudget(clientId, currentBudget, desiredBudget);
      return;
    }
  

    /** 
     * Es posible que tengamos operaciones pendientes por culpa de otras 
     * aplicaciones nuevas que hayan entrado y no tengan suficiente budget
     */
    {
	
	bool earlyExit=false;

	auto it = _changesToCommit.find(clientId);
	while(it != _changesToCommit.end()){

	    float requiredBudget; int clientRequired;
	    std::tie(clientRequired, requiredBudget) = it->second;


	    const float assignedBudget = std::min(requiredBudget, cb.given);
	    cb.given -= assignedBudget;

	    _budgetInfo[clientRequired].given += assignedBudget;

	    auto it2=_toNotify.find(clientRequired);
	    if( it2!= _toNotify.end()){
	      std::cout << "despertando a " << *it2 << std::endl;
	      _srv->sendNewBudget(*it2, _budgetInfo[*it2].given);
	      _toNotify.erase(it2);
	    }
	    
	    earlyExit = true;
	    _changesToCommit.erase(it);
	    it = _changesToCommit.find(clientId);
	}

	if(earlyExit){
	    _srv->sendNewBudget(clientId, cb.given);
	    return;
	}
    }

    

    /** 
     * La aplicacion puede ser de 3 tipos:
     *  (a) Le sobra budget.    [given > used] == [given-used > margin]
     *  (b) Ni le sobra ni le falta [used ~= required] == [ abs(used-required) <= margin ]
     *  (c) Necesita más budget [required > given] == [required-given > margin]
     **/

    
    /* (A) Le sobra budget */
    if( cb.given - cb.used > _budgetMargin) { 
	const float oldBudget = cb.given;
	const float newBudget = (cb.used == 0) ? cb.given : cb.used;
	
	_availableBudget += oldBudget - newBudget;

	cb.given = newBudget;
	_srv->sendNewBudget(clientId, newBudget);

        #ifdef DEBUG
	  std::stringstream ss ("");
	  ss << "[" << clientId << "] -- A --  [" <<
	      cb.given << ", " <<  cb.used << ", " << cb.required << '\n';
	  Log::DEBUG_MSG(ss.str());
        #endif
	return;
    }
    /* (B) Ni le sobra ni le falta */
    else if( std::fabs(cb.required-cb.given) <= _budgetMargin ) {
	_srv->sendNewBudget(clientId, cb.given);
        #ifdef DEBUG
	  std::stringstream ss ("");
	  ss << "[" << clientId << "] -- C --  [" <<
	      cb.given << ", " <<  cb.used << ", " << cb.required << '\n';
	  Log::DEBUG_MSG(ss.str());
        #endif
	return;
    }
    /* (C) necesita todo el budget que le demos */
    else if( cb.required - cb.given > _budgetMargin ){
	
	if(cb.given + _availableBudget > cb.required){ //no cogemos todo
	    _availableBudget -= (cb.required-cb.given);
	    cb.given = cb.required;
	} else { //cogemos todo lo disponible
	    cb.given += _availableBudget;
	    _availableBudget=0;
	}

	_srv->sendNewBudget(clientId, cb.given);
        #ifdef DEBUG
	  std::stringstream ss ("");
	  ss << "[" << clientId << "] -- C --  [" <<
	      cb.given << ", " <<  cb.used << ", " << cb.required << '\n';
	  Log::DEBUG_MSG(ss.str());
        #endif
	return;
	
    } else {

      std::cerr <<  cb.given << "  --  " << currentBudget << "  --  " <<  desiredBudget << std::endl;
	Log::WARNING_MSG("CASO NO CONTEMPLADO");
	abort();
    }
    
	
	
    
    
    
    auto ci = _srv->getClientInfo();

    //A) Update current budget
    ci->at(clientId)->setCurrentBudget(currentBudget);
    ci->at(clientId)->setDesiredBudget(desiredBudget);
    
    //B) Get all desired budgets
    float globalDesiredBudget=0.0;
    for(const auto &it : *ci){
	globalDesiredBudget += it.second->getDesiredBudget();
    }

   
    //C) Give each client budget proportionally to the required budget
    float newBudget = (_globalBudget / globalDesiredBudget) * desiredBudget;

    
    _srv->sendNewBudget(clientId, newBudget);
   
}
