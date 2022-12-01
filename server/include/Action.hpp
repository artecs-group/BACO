#ifndef _ACTION_HPP_
#define _ACTION_HPP_

#include "common/dynamic_bitset.hpp"
#include "Client.hpp"
#include "ClientInfo.hpp"

class Server;
 
class Action
{
public:
    /* Ctrs and dtrs */
    Action(Server* srv);
    virtual ~Action();

    Action() = delete;
    Action(const Action&) = delete;
    Action& operator=( const Action&) = delete;

    
    /* Methods that all actions have to implement */
    virtual const char* getName() const = 0;

    
    virtual void registerClient(int id);
    virtual void startClient(int clientId);
    virtual void unregisterClient(Client *cl, ClientInfo *ci);


    virtual void updateNTasks(int clientId, int nCrit, int nNotCrit);
    virtual void setNewCpuMask(int clientId);
    virtual void confirmCpuMask(int clientId, dynamic_bitset& bt);
    virtual void confirmCoreFree(int clientId, int coreId);

    virtual void requestNewBudget(int clientId, float currentBudget, float desiredBudget);
    
protected:
    Server *_srv;
    
};

#endif /* _ACTION_HPP_ */
