
#include "ns3/command-line.h"
#include "ns3/core-module.h"
#include "ns3/simulator.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/node.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/udp-server.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/ipv4-global-routing-helper.h"

#include "ns3/data-rate.h"
#include "ns3/channel.h"

#include <string>
#include "ns3/csma-helper.h"

#include "ns3/on-off-helper.h"
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/application-container.h"
#include "observador.h"
#include "ns3/address.h"
#include"puentehelper.h"
#include "ns3/inet-socket-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/queue.h"

#include "ns3/csma-net-device.h"
#include "ns3/names.h"

#include "ns3/gnuplot.h"
#include "retardo.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Practica05");

void escenario(uint32_t num_fuentes, double n_max_paq, Time duracion_simulacion, Time int_envio_cliente, double tam_paq, DataRateValue c_transmision, double tam_cola, double error, Gnuplot2dDataset* curva_perdidas, Gnuplot2dDataset* curva_retardo);

int main(int argc, char *argv[])
{
    Time::SetResolution(Time::NS);
  
    uint32_t num_fuentes= ( 6 + (3 % 3) );  // Último dígito del número de grupo. 
    std::string int_envio_cliente_value = "100us";  //Por defecto es el intervalo mínimo, cercano al teórico al que no se pierden paquetes.
    double tam_paq_value = 1024;
    double tam_cola_inicial = 50;
    int num_curvas = 4;
    int num_puntos = 5; // Numero de veces que se decrementará la cola del conmutador.
    double error = 2;
    double n_max_paq = ( 400 + 100*( 13 % 4 ) ); //13 -> número de grupo

    //Capacidad de los enlaces
    


}



