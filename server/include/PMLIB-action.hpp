#ifndef _PMLIB_ACTION_HPP_
#define _PMLIB_ACTION_HPP_

#include "common/utils.hpp"

#ifdef PMLIB

#include "Action.hpp"
extern "C"
{
  #include "pmlib.h"
}


class PmlibAction : public Action
{
public:
    /* Ctrs and dtrs */
    PmlibAction(Server* srv);
    ~PmlibAction();

    /* Methods that all actions have to implement */
    const char* getName() const override;

    void registerClient(int id) override;
    void unregisterClient(Client *cl, ClientInfo *ci) override;

  
 private:
    server_t   _pmlibServer;
    counter_t  _pmlibCounter;
    line_t     _pmlibLines;

    const int _frequency = 0;
    const int _aggregate = 0;

    bool _started;
};




#endif // PMLIB
#endif /* _PMLIB_ACTION_HPP_ */

