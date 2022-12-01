#include "PMLIB-action.hpp"
#ifdef PMLIB

#include <stdlib.h>
#include "Server.hpp"

const char* PmlibAction::getName() const
{
    return "Pmlib Action";
}


PmlibAction::PmlibAction(Server* srv) : Action(srv), _frequency(100), _aggregate(0), _started(false)
{
    //Inicializar la comunicacion con el servidor:
    // Preparacion de los contadores, lineas, etc

    pm_set_server("127.0.0.1", 6526, &_pmlibServer);

    LINE_CLR_ALL(&_pmlibLines);
    LINE_SET_ALL(&_pmlibLines);

#if defined(BUJARUELO) || defined(bujaruelo)
    pm_create_counter("RAPLDevice", _pmlibLines, _aggregate, _frequency, _pmlibServer, &_pmlibCounter);
#endif

#if defined(eslsrv10) || defined(ESLSRV10)
    pm_create_counter("RAPLDevice", _pmlibLines, _aggregate, _frequency, _pmlibServer, &_pmlibCounter);
#endif

#if defined(makalu) || defined(MAKALU) || defined(volta1) || defined(VOLTA1)
    pm_create_counter("RAPLDevice", _pmlibLines, _aggregate, _frequency, _pmlibServer, &_pmlibCounter);
#endif

}


PmlibAction::~PmlibAction()
{
    //Generar el fichero con los datos, y eliminar estrucuras del servidor
    pm_get_counter_data( &_pmlibCounter );


    char *energy_filename = getenv("PMLIB_OUT");

    if(energy_filename == NULL){
	fprintf(stderr,"+++++++++++++++++++++++++++++ pmlib_energy.csv [PMLIB_OUT==NULL] ++++++++++++++++++++++++++++++");
	pm_print_data_csv("pmlib_energy.csv", _pmlibCounter, _pmlibLines, -1);
	pm_print_data_paraver("pmlib_energy.prv", _pmlibCounter, _pmlibLines, -1, "us");
    }
    else {
      fprintf(stderr,"+++++++++++++++++++++++++++++ %s ++++++++++++++++++++++++++++++", energy_filename);
      pm_print_data_csv(energy_filename, _pmlibCounter, _pmlibLines, -1);
    }
    pm_finalize_counter( &_pmlibCounter );

}


void PmlibAction::registerClient(int id __attribute__ ((unused)) )
{
  //Si no habia ningun cliente registrado, comenzar a medir
  if(_srv->getClientInfo()->size() == 1){
    if(!_started){
      _started=true;
      pm_start_counter(&_pmlibCounter);
    } else {
      pm_continue_counter(&_pmlibCounter);
    }
  }
}


void PmlibAction::unregisterClient(Client *cl         __attribute__ ((unused)),
				   ClientInfo *ci     __attribute__ ((unused)) )
{
    //Si es el ultimo cliente, parar el contador
    if(_srv->getClientInfo()->size() == 0)
	pm_stop_counter(&_pmlibCounter);
}


#endif //PMLIB
