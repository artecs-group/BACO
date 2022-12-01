#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include "Action.hpp"

#include "ClientSupervisor.hpp"
#include "Client.hpp"

#include "ClientInfo.hpp"
#include "SysInfo.hpp"

#include "common/dynamic_bitset.hpp"


#include <map>
#include <thread>
#include <mutex>
#include <vector>


/**
 * Se asume que el punto de entrada a la información de los clientes
 * (Client* y ClientInfo*) es a través del servidor, ya sea desde un
 * cliente, el ClientSupervisor o una acción. Por lo tanto, es el
 * servidor el responsable de adquirir y liberar el mutex antes de que
 * alguien haga algún cambio en los datos. De esta manera, el resto de
 * clases pueden asumir que el acceso a la información está protegido
 * y pueden operar con normalidad.
 */

class Server
{
public:
    Server();
    virtual ~Server();

    virtual void registerAction(Action *act, int priority);
    virtual void Run();
    
    virtual void registerClient(int id, Client* cl);
    virtual void processStartRequest(int id);  
    virtual void unregister(int id);

    virtual void updateNtasks(int clientId, int nCrit, int nNotCrit);
    virtual void sendNewCpuMask(int clientId);
    virtual void confirmCpuMask(int clientId, dynamic_bitset& bt);
    virtual void confirmCoreFree(int clientId, int coreId);

    virtual void processMsgRequestBudget(int clientId, float currentBudget, float desiredBudget);
    
    virtual void stop();


    /* Estos métodos no deberían ser invocados directamente, sino como
       resultado de alguna llamada anterior, por lo que se asume que
       el mutex ya esta adquirido y no es necesario volverlo a adquirir
    */
    virtual void startClient(int id);    
    virtual void notifyNewCpuMask(int id);
    virtual void sendReleaseCoreOrder(int clientId);
    virtual void sendReducedMaskMsg(int clientId, const dynamic_bitset &bt);
    virtual void sendNewBudget(int clientId, float newBudget);
    
    
    
    std::map<int, ClientInfo*>* getClientInfo();
    SysInfo* getSysInfo();

    
protected:
    void insertClient(int id, Client *cl, ClientInfo *clInfo);

    ClientSupervisor *_clSp;
  
    std::mutex _clientsMtx;
    std::map<int, Client*> _clients;
    std::map<int, ClientInfo*> _clientsInfo;
  
    SysInfo *_sysInfo;
private:

    std::vector< std::pair<int, Action*> > _actions;
};


#endif /* _SERVER_HPP_ */
