#ifndef _COMUNICATION_MANAGER_HPP_
#define _COMUNICATION_MANAGER_HPP_

#include "common/msg.hpp"


class CommunicationManager
{
public:
    CommunicationManager();
    ~CommunicationManager();

    int registerClient(int id);
    int unregisterClient();

    /*   SIZE WITH type field included!!!!                server/client  */
    int sendMsg(int size, void *msg, bool wait=true, bool server=false);
    int rcvMsg(void *msg, message_t msgType=message_t::Raw, bool wait=true);

    int closeQueues();

private:
    int createClientQueue(int id=-1);

    int _fdServer;
    int _fdClientIn;
    int _fdClientOut;
    
};

#endif /* _COMUNICATION_MANAGER_HPP_ */
