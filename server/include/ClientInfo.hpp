#ifndef _CLIENT_INFO_HPP_
#define _CLIENT_INFO_HPP_

#include "common/dynamic_bitset.hpp"

#include <thread>

class ClientInfo
{
public:
    ClientInfo() = delete;
    ClientInfo(int id);
    ~ClientInfo();

    //Constant elements
    int getId() const;

    void set_activeMask(dynamic_bitset bt);
    dynamic_bitset get_activeMask() const;
    dynamic_bitset& get_activeMask();

    void set_futureMask(dynamic_bitset bt);
    dynamic_bitset get_futureMask() const;
    dynamic_bitset& get_futureMask();


    void setCurrentBudget(float currentBudget);
    void setDesiredBudget(float desiredBudget);
    float getDesiredBudget() const;

    
private:
    //static info
    const int _id;

    // client info
    int _ntask_crit;
    int _ntask_notCrit;
    
    dynamic_bitset _activeCores; //granted Cores
    dynamic_bitset _futureCores; //cores that are allow to use, but in the future
                                 //  because currently they are used by another client


    float _currentBudget;
    float _desiredBudget;


#ifdef DEBUG
    unsigned long long _registrationTimeStamp;
#endif

};


#endif /* _CLIENT_INFO_HPP_ */


