#ifndef _CLIENT_SUPERVISOR_HPP_
#define _CLIENT_SUPERVISOR_HPP_


#include "CommunicationManager.hpp"
#include "common/msg.hpp"
//#include "Server.hpp"
class Server;

class ClientSupervisor
{
public:
    ClientSupervisor() = delete;

    ClientSupervisor(Server *srv, int key_id, const char* key_path);
    ~ClientSupervisor();

    void Run();
    void stop();
private:
    void registerClient(msg_registration_t *msg);

    Server *_srv;
    CommunicationManager *_com;
    bool _end;
};



#endif /* _CLIENT_SUPERVISOR_HPP_ */
