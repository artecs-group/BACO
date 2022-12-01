#ifndef _MALLEABLE_FREQ_ACTION_HPP_
#define _MALLEABLE_FREQ_ACTION_HPP_

#include "common/dynamic_bitset.hpp"
#include "Client.hpp"
#include "ClientInfo.hpp"
#include "Action.hpp"

#include <mutex>
#include <vector>
#include <queue>
#include <map>
#include <cmath>
#include <set>

class MalleableFreqAction : public Action
{
public:
    /* Ctrs and dtrs */
    MalleableFreqAction(Server* srv);
    virtual ~MalleableFreqAction();

    MalleableFreqAction() = delete;
    MalleableFreqAction(const MalleableFreqAction&) = delete;
    MalleableFreqAction& operator=( const MalleableFreqAction&) = delete;

    
    /* Methods that all actions have to implement */
    virtual const char* getName() const override;

    virtual void registerClient(int id) override;
    virtual void startClient(int clientId) override;
    virtual void unregisterClient(Client *cl, ClientInfo *ci) override;

    // virtual void updateNTasks(int clientId, int nCrit, int nNotCrit) override;
    // virtual void setNewCpuMask(int clientId) override;
    // virtual void confirmCpuMask(int clientId, dynamic_bitset& bt) override;
    // virtual void confirmCoreFree(int clientId, int coreId) override;

    virtual void requestNewBudget(int clientId, float currentBudget, float desiredBudget) override;


private:
    /** Cuando el cliente se registra por primera vez (currentBudget==0), esta función le da
	budget de donde puede: o se lo da del budget available, o se lo quita a otro cliente **/
    virtual void giveFirstBudget(int clientId, float currentBudget, float desiredBudget);
  
private:
    class budgetInfo_t {
    public:
	float given=0.0;
	float used=0.0;
	float required=0.0;

	budgetInfo_t() : given(0.0), used(0.0), required(0.0){};
    };
    

private:

    float _globalBudget=125.0; /* Budget to share. READ FROM ENVIRONMEWNT VARIABLE */
    const float _budgetMargin; /* Numero de vatios que consideramos de margen 
				  antes de decidir si quitarle o darle más budget */

    float _availableBudget; /* Budget disponible para repartir*/
    std::map<int, budgetInfo_t> _budgetInfo; /* Informacion de los clientes con 
						el budget dado, usado y requerido */

    
    std::map<int, std::pair<int, float> > _changesToCommit; /* Si hay algún valor en este 
							       mapa, significa que la aplicacion
							       tiene que una aplicación tiene que
							       darle budget a otra, y por lo tanto,
							       ignorar cualquier otro cambio que 
							       fuera a hacer */

    std::set<int> _toNotify;



    //NYAPA PARA SUPORTAR RUNNING APPLICATIONS
    const unsigned int _NCORES;
//    const double _idlePower;
};

#endif /* _MALLEABLE_FREQ_ACTION_HPP_ */
