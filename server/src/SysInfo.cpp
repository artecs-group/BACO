#include "SysInfo.hpp"
#include "common/utils.hpp"
#include "LogHelper.hpp"
#include <unistd.h>


SysInfo::SysInfo()
{

#if defined(MULHACEN) || defined(mulhacen)
  _numCpus = 68;
#elif defined(MAKALU) || defined(makalu) || defined(VOLTA1) || defined(volta1)
  _numCpus = 20;
#else
  _numCpus = sysconf(_SC_NPROCESSORS_ONLN);
#endif

  _activeMask = dynamic_bitset(_numCpus, false);
}

SysInfo::~SysInfo()
{
}


int SysInfo::getNumCpus() const
{
    return _numCpus;
}


int SysInfo::getNumActiveCores() const
{
    return _activeMask.count();
}

int SysInfo::getNumIdleCores() const
{
    return _numCpus - getNumActiveCores();
}

dynamic_bitset SysInfo::getAllCpusMask() const
{
    return dynamic_bitset(_numCpus, true);
}


dynamic_bitset SysInfo::getActiveMask()
{
    return _activeMask;
}


dynamic_bitset SysInfo::getIdleMask()
{
    return ~_activeMask;
}


int SysInfo::setActiveCores(dynamic_bitset db)
{
    if( db.none() ) return 0;
    
    int count = _activeMask.count();

    _activeMask |= db;
        
    return (count == _activeMask.count()) ? -1 : 0;
}


int SysInfo::setActiveCore(int coreId)
{
    if(_activeMask.test(coreId)) return -1;
    
    _activeMask.set(coreId, true);
    return 0;
}


int SysInfo::setIdleCores(dynamic_bitset bt)
{
    if( bt.none() ) return 0;

    
    int count = _activeMask.count();

    _activeMask &= (~bt);

    return (count == _activeMask.count()) ? -1 : 0;
    
}


int SysInfo::setIdleCore(int cpuId)
{
    if( !_activeMask.test(cpuId)) return -1;
    
    _activeMask.set(cpuId, false);
    return 0;

}



dynamic_bitset SysInfo::giveMeNIdleCores(int n) const
{
    dynamic_bitset bt(_numCpus, false);
    
    dynamic_bitset idleMask = ~_activeMask;
    
    int idx=idleMask.find_first();
    bt.set(idx, true);
    
    for(int i=1; i<n; i++){
	idx = idleMask.find_next(idx);
	bt.set(idx, true);
    }
    
    return bt;
}



// #include <iostream>
// int main()
// {
//     SysInfo si;
//     std::cout << si.getActiveMask().to_string() << std::endl;
//     std::cout << si.getIdleMask().to_string() << std::endl;
//     puts("");
    
//     si.setActiveCore(3);
//     std::cout << si.getActiveMask().to_string() << std::endl;
//     std::cout << si.getIdleMask().to_string() << std::endl;
//     puts("");
    
//     si.setActiveCore(1);
//     std::cout << si.getActiveMask().to_string() << std::endl;
//     std::cout << si.getIdleMask().to_string() << std::endl;
//     puts("");
    
    
    
//     return 0;
// }
