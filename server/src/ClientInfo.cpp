#include "ClientInfo.hpp"

#include "LogHelper.hpp"
#include "common/utils.hpp"

#ifdef DEBUG
#include <cassert>
#endif

ClientInfo::ClientInfo(int id) :
    _id(id), _ntask_crit(0), _ntask_notCrit(0), _currentBudget(0.0), _desiredBudget(0.0)
{
    ONLY_DBG( _registrationTimeStamp = getTimeStamp() );
}

ClientInfo::~ClientInfo(){

#ifdef DEBUG
  char txt[200];
  sprintf(txt, "Client %id, execution time: %llu\n", _id, getTimeStamp() - _registrationTimeStamp);

  Log::INFO_MSG(txt);
#endif
}


int ClientInfo::getId() const
{
    return _id;
}


void ClientInfo::set_activeMask(dynamic_bitset bt)
{
    _activeCores=bt;
}

dynamic_bitset ClientInfo::get_activeMask() const 
{
    return _activeCores;
}

dynamic_bitset& ClientInfo::get_activeMask() 
{
    return _activeCores;
}

void ClientInfo::set_futureMask(dynamic_bitset bt)
{
    _futureCores = bt;
}

dynamic_bitset ClientInfo::get_futureMask() const
{
    return _futureCores;
}

dynamic_bitset& ClientInfo::get_futureMask()
{
    return _futureCores;
}



void ClientInfo::setCurrentBudget(float currentBudget)
{
    _currentBudget=currentBudget;
}

void ClientInfo::setDesiredBudget(float desiredBudget)
{
    _desiredBudget=desiredBudget;
}

float ClientInfo::getDesiredBudget() const
{
    return _desiredBudget;
}


//------------------------------------------------------------------------------

// int ClientInfo::getNumCoresRequested() const
// {
//     return _nCoresRequested;
// }

// void ClientInfo::decreaseNCoresRequested(int nThreads)
// {
//     _nCoresRequested -= nThreads;
//     if(_nCoresRequested < 0 ) _nCoresRequested = 0;
// }

// void ClientInfo::increaseNCoresRequested(int nTrheads)
// {
//     _nCoresRequested += nTrheads;
//     if(_nCoresRequested > _maxWorkers) _nCoresRequested = _maxWorkers;
// }


// int ClientInfo::wantMoreCores()
// {
//     if(_nCoresRequested > _cpuMask.count())
// 	return _nCoresRequested - _cpuMask.count();
//     else
// 	return 0;
// }


// void ClientInfo::setActiveMask(dynamic_bitset mask)
// {
//     _cpuMask = mask;
// }


// dynamic_bitset ClientInfo::getActiveMask() const
// {
//     return _cpuMask;
// }

// ClientInfo::status_t ClientInfo::getStatus() const
// {
//     return _status;
// }


// void ClientInfo::setStatus(ClientInfo::status_t st)
// {
//     _status = st;
// }


