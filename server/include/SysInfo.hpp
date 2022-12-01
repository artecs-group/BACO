#ifndef _SYS_INFO_HPP_
#define _SYS_INFO_HPP_

#include "common/dynamic_bitset.hpp"


class SysInfo
{
public:
    SysInfo();
    ~SysInfo();

    // //Related with active cores
    dynamic_bitset getAllCpusMask() const;    
    dynamic_bitset getActiveMask();
    dynamic_bitset getIdleMask();

    int setActiveCores(dynamic_bitset bt); //returns -1 if we you try to activate an alredy active core, 0 o.w.
    int setActiveCore(int coreId); //returns -1 if we you try to activate an alredy active core, 0 o.w.

    int setIdleCores(dynamic_bitset bt); //return -1 if you try to put in idle an alredy idle core, 0 o.w.
    int setIdleCore(int coreId); //return -1 if you try to put in idle an alredy idle core, 0 o.w.
    
    dynamic_bitset giveMeNIdleCores(int n) const;

    
    int getNumCpus() const;

    inline int getNumActiveCores() const;
    inline int getNumIdleCores() const;
    
private:
    int _numCpus;    
    dynamic_bitset _activeMask;

    // int _nActiveCores;
};

#endif /* _SYS_INFO_HPP_ */
