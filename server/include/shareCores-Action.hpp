#ifndef _SHARE_CORES_ACTION_HPP_
#define _SHARE_CORES_ACTION_HPP_

#include "common/dynamic_bitset.hpp"
#include "Client.hpp"
#include "ClientInfo.hpp"
#include "Action.hpp"

#include <mutex>
#include <vector>
#include <queue>


class ShareCoresAction : public Action
{
public:
    /* Ctrs and dtrs */
    ShareCoresAction(Server* srv);
    virtual ~ShareCoresAction();

    ShareCoresAction() = delete;
    ShareCoresAction(const ShareCoresAction&) = delete;
    ShareCoresAction& operator=( const ShareCoresAction&) = delete;

    
    /* Methods that all actions have to implement */
    virtual const char* getName() const override;

    
    virtual void registerClient(int id) override;
    virtual void startClient(int clientId) override;
    virtual void unregisterClient(Client *cl, ClientInfo *ci) override;

    virtual void updateNTasks(int clientId, int nCrit, int nNotCrit) override;
    virtual void setNewCpuMask(int clientId) override;
    virtual void confirmCpuMask(int clientId, dynamic_bitset& bt) override;
    virtual void confirmCoreFree(int clientId, int coreId) override;
    
private:
    void recalculate_coreDistribution();
    void distributeCores_equal();

    void orderToFreeCores(int clientId, int nCores=1);
    void assignFreeCores();
    void freeCore(int i);

    void updateStatus();
    
    inline int getNumIdleCores() const;
    inline int getNumWaiting2RunCores() const;
    void removeClientFromWaitingQueue(int clientId);
    

private:
    enum CoreStatus{
	IDLE, RUNNING, WAITING_TO_RUN
    };


    /*** mtx!!!!! ***/
    std::recursive_mutex _changeInProgress_mtx; 
    bool _changeInProgress;
    bool _futureChange;
    

    /*** mtx!!!!! ***/
    std::mutex _status_mtx;
    std::vector<CoreStatus> _coreStatus;
                           //<numCores, clientId>
    std::priority_queue<std::pair<int, int>> _clientsWaitingCores; //std::less sort by default
};

#endif /* _SHARE_CORES_ACTION_HPP_ */
