# BACO
Este repositorio contiene la implementación de BACO, un gestor de recursos global capaz de comunicarse con multiples aplicaciones de forma simultánea y repartir los recursos disponibles.

El repositorio está formado por dos componentes principales:


## Servidor
Es la implementación del gestor de recursos. Soporta una implementación genérica donde distintas acciones se pueden registrar y desregistrar para implementar distintas políticas.
Cada vez que se desencadena un evento en el servidor, éste es procesado por las acciones registradas en un orden específico.

El fichero [server/src/main.cpp](server/src/main.cpp) muestra como registrar distintas acciones en el sistema. Además, los fichero (\*-Action.cpp) de la carpeta [server/src/] muestran ejemplos de como implementar estas acciones.
Toda acción debe implementar/heredar la interfaz definida en [server/include/Action.hpp](server/include/Action.hpp)


## Cliente
Implementa una libería para comunicación con el servidor. 
La comunicación es totalmente transparente entre servidor y cliente. (La implementación actual usa colas de mensajes SYS-V).
Los métodos disponibles para el cliente están en [client/include/client.h](client/include/client.h).

Aunque la implementación está en C++, la librería resultante se puede enlazar e invocar desde un código en C


# Acknowledgements

This work partially supported by the EU (FEDER) and Spanish MINECO (TIN20215-65277-R, RTI2018-093684-B-I00), MICINN (PID2021-126576NB-I00), CM (S2018/TCS-4423), MECD (FPU15/02050), and the Madrid Government (Comunidad de Madrid- Spain) under the Multiannual Agreement with Complutense University in the line Program to Stimulate Research for Young Doctors in the context of the V PRICIT (Regional Programme of Research and Technological Innovation) under project PR65/19-22445.
