#include "Action.hpp"


Action::Action(Server *srv): _srv(srv)
{}


Action::~Action()
{}


// --------------
void Action::registerClient(int id                    __attribute__ ((unused)) )
{}


void Action::startClient(int clientId                 __attribute__ ((unused)))
{}

void Action::unregisterClient(Client *cl              __attribute__ ((unused)),
			      ClientInfo *ci          __attribute__ ((unused)) )
{}



// --------------
void Action::updateNTasks(int clientId                __attribute__ ((unused)),
			  int nCrit                   __attribute__ ((unused)),
			  int nNotCrit                __attribute__ ((unused)) )
{}


void Action::setNewCpuMask(int clientId               __attribute__ ((unused)) )
{}

void Action::confirmCpuMask(int clientId              __attribute__ ((unused)),
			    dynamic_bitset& bt        __attribute__ ((unused)) )
{}



void Action::confirmCoreFree(int clientId              __attribute__ ((unused)),
			     int coreId                __attribute__ ((unused)))
{}


void Action::requestNewBudget(int clientId             __attribute__ ((unused)),
			      float currentBudget      __attribute__ ((unused)),
			      float desiredBudget      __attribute__ ((unused)))
{}

