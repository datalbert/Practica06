
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
#include "ns3/address.h"
#include"puentehelper.h"
#include "ns3/inet-socket-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/queue.h"

#include "ns3/csma-net-device.h"
#include "ns3/names.h"

#include "ns3/gnuplot.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Practica06");

void escenario(uint32_t num_fuentes, Time duracion_simulacion, Time int_envio_cliente, double tam_paq,DataRateValue tasa_envio);


    
int main(int argc, char *argv[])
{
    Time::SetResolution(Time::NS);
  
    //Número máximo de paquetes a enviar: los suficientes para que se produzca el llenado.
    
    int num_fuentes=2;
    double tam_paq_value=1024;
    std::string tasa_envio_dispositivos="4Mbps";
    std::string int_envio_cliente_value="2ms";
    Time duracion_simulacion=Time("10s");
    //Capacidad de los enlaces
    std::string c_transmision_value = "100Mbps";
    //-----------------------------------------------------------

    // Configuracion linea de comandos
    CommandLine cmd(__FILE__);

    cmd.AddValue("num_fuentes", "Numeros de fuentes del escenario a crear", num_fuentes);
    cmd.AddValue("int_envio_cliente", "Intervalo de envio de paquetes del cliente", int_envio_cliente_value);
    cmd.AddValue("tam_paq", "Tamano del paquete ", tam_paq_value);
    cmd.AddValue("tasa_envio", "Tasa de envio de los dispositivos ", tasa_envio_dispositivos);
    cmd.Parse(argc, argv);


    //Tenemos una cola en cada disposiutivo a nivle de enlace y otra a nivel ip cuando la cola de dispositivo 

    Time int_envio_cliente=Time(int_envio_cliente_value);
    DataRateValue tasa_envio=DataRateValue(tasa_envio_dispositivos);

    escenario(num_fuentes,duracion_simulacion,int_envio_cliente,tam_paq_value,tasa_envio,tasa_envio);


        
        
    
}



void escenario(uint32_t num_fuentes, Time duracion_simulacion, Time int_envio_cliente, double tam_paq,DataRateValue tasa_envio)

{

    NodeContainer clientes_c(num_fuentes);

    //Creacion de nodo sumidero
    Ptr<Node> sumidero=CreateObject<Node>();

    NodeContainer c_todos;
    c_todos.Add(sumidero);
    c_todos.Add(clientes_c);

    InternetStackHelper h_pila;
    h_pila.SetIpv6StackInstall(false);
    h_pila.Install(c_todos);

    NetDeviceContainer c_dispositivos;

    PuenteHelper puent;
    Ptr<Node> puente=CreateObject<Node>();
    puente=puent.Puentehelper(c_todos,c_dispositivos,tasa_envio     );

    //Configuracion de la capa de aplicacion del sumidero

    uint16_t puerto=20;
    UdpServerHelper sum_udp(puerto);
    ApplicationContainer app_container_sumidero=sum_udp.Install(sumidero); 

    Ipv4AddressHelper h_direcciones ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer c_interfaces =h_direcciones.Assign (c_dispositivos);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    
    //Capa de aplicación para las fuentes 
    
    DataRate data_rate = DataRate(( tam_paq * 8 ) / int_envio_cliente.GetSeconds());
    NS_LOG_INFO("Régimen Binario de las fuentes : " << data_rate);

    InetSocketAddress dir(c_interfaces.GetAddress(0,0),puerto);
    OnOffHelper onoff("ns3::UdpSocketFactory",dir);
    onoff.SetConstantRate(data_rate,tam_paq);
    ApplicationContainer app_c=onoff.Install(clientes_c);
    for (uint64_t i = 0; i < num_fuentes; i++){
        //app_c.Get(i)->GetObject<OnOffApplication>()->SetMaxBytes(n_max_paq*tam_paq);
        app_c.Get(i)->SetStopTime(duracion_simulacion);
    }    

   
    
    NS_LOG_INFO("Arranca Simulación");
    Simulator::Stop(duracion_simulacion);
    Simulator::Run();
    NS_LOG_DEBUG("En recepcion");
    NS_LOG_INFO("Fin simulación");

    
    Simulator::Destroy();

}