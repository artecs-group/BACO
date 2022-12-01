#include "Server.hpp"
#include "LogHelper.hpp"

#include <signal.h>
#include <unistd.h>
#include <cstring>


#include "PMLIB-action.hpp"
#include "shareCores-Action.hpp"
#include "MalleableFreq-Action.hpp"


//variable global para poder parar el servicio desde la rutina de tratamiento de la interrupción
Server* s;


static void sig_handler(int signo)
{
    if(signo != SIGINT)
	puts("OJO; que la señal capturada no es la que esperabamos");

    s->stop();
}


//Action* parseArguments(int argc, char** argv);


//int main(int argc, char** argv)
int main()
{
    Log::VERBOSE_MSG("init main");
    s =  new Server();

    Action *shc  = new ShareCoresAction(s);
    Action *mfa = new MalleableFreqAction(s);
    ONLY_PMLIB( Action *acpmlib = new PmlibAction(s) );

    s->registerAction(shc,  1);
    s->registerAction(mfa,  2);
    ONLY_PMLIB( s->registerAction(acpmlib, 99));

    //campturamos la señal para fin
    if(signal(SIGINT, sig_handler) == SIG_ERR) {
	DIE(__FILE__ ": Error al registrar la rutina de tratamiento de la interrupción SININT");
	return EXIT_FAILURE; 
    }

    
#ifdef DEBUG
    fprintf(stderr, "===================================\n");
    fprintf(stderr, "= ProcessId:    %d\n", getpid());
    fprintf(stderr, "= CreationTime: %llu\n", getTimeStamp());
    fprintf(stderr, "= ActionUsed: %s\n",     mfa->getName());
    fprintf(stderr, "===================================\n");
#endif
    
    // RUN
    s->Run();
    
    /// TODAS LAS ACCIONES SON LIBERADAS EN EL SERVIDOR
    delete s;
    delete Log::LogHelper::Singleton();

    
    return EXIT_SUCCESS;
}

