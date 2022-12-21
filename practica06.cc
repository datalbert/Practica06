
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

#include "ns3/traffic-control-layer.h"
#include "ns3/traffic-control-helper.h"
#include "ns3/queue-disc.h"
#include "ns3/queue-disc-container.h"

#include "ns3/gnuplot.h"
#include "retardo.h"
#include "cola_observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Practica06");

void escenario(uint32_t num_fuentes, double n_max_paq, Time duracion_simulacion, Time int_envio_cliente, double tam_paq, DataRateValue c_transmision, double tam_cola);

int main(int argc, char *argv[])
{
    Time::SetResolution(Time::NS);
  
    uint32_t num_fuentes= 2;  // Último dígito del número de grupo. 
    std::string int_envio_cliente_value = "1ms";  //Por defecto es el intervalo mínimo, cercano al teórico al que no se pierden paquetes.
    double tam_paq_value = 8192;
    double tam_cola_inicial = 50;
    int num_curvas = 4;
    int num_puntos = 5; // Numero de veces que se decrementará la cola del conmutador.
    double error = 2;
    double n_max_paq = ( 400 + 100*( 13 % 4 ) ); //13 -> número de grupo

    //Capacidad de los enlaces
    std::string c_transmision_value = "1Mbps";
    //-----------------------------------------------------------

    // Configuracion linea de comandos
    CommandLine cmd(__FILE__);

    cmd.AddValue("num_curvas", "Veces que aumentará el número de fuentes", num_curvas);
    cmd.AddValue("num_puntos", "Veces que se decrementará la cola de paquetes", num_puntos);
    cmd.AddValue("num_fuentes", "Numeros de fuentes del escenario a crear", num_fuentes);
    cmd.AddValue("int_envio_cliente", "Intervalo de envio de paquetes del cliente", int_envio_cliente_value);
    cmd.AddValue("tam_paq", "Tamano del paquete UDP", tam_paq_value);
    cmd.AddValue("tam_cola", "Tamano de cola en número de paquetes", tam_cola_inicial);
    cmd.AddValue("error", "Error en las gráficas", error);
    cmd.AddValue("c_transmision", "Capacidad de transmision de los enlaces del conmutador", c_transmision_value);
    cmd.AddValue("n_max_paq", "Número máximo de paquetes a enviar", n_max_paq);
    
    cmd.Parse(argc, argv);

    DataRateValue c_transmision = DataRateValue(c_transmision_value);
    Time int_envio_cliente = Time(int_envio_cliente_value);

    //Duración de la simulación acorde al número máximo de paquetes a enviar.
    Time duracion_simulacion = Time( n_max_paq * int_envio_cliente + Time("10ms") ); //Tiempo prudencial para establecer conexión

    //Gráfica -> Pérdida de paquetes entre tamano de cola
    /*std::string fileNameWithNoExtension_perdidas = "practica05_perdidas";
    std::string graphicsFileName_perdidas= fileNameWithNoExtension_perdidas + ".png";
    std::string plotFileName_perdidas= fileNameWithNoExtension_perdidas + ".plt";
    std::string plotTitle_perdidas="Porcentaje Paquetes perdidos en funcion del tamaño de cola";
    
    Gnuplot grafico_perdidas(plotFileName_perdidas);
    grafico_perdidas.SetTitle (plotTitle_perdidas);
    grafico_perdidas.SetLegend ("Tamaño cola servidor (pckts)"," % de Paquetes perdidos");
    std::ofstream fichero_perdidas(plotFileName_perdidas);
    
    //Gráfica -> Retardo medio entre tamano de cola
    std::string fileNameWithNoExtension_retardo = "practica05_retardo";
    std::string graphicsFileName_retardo= fileNameWithNoExtension_retardo + ".png";
    std::string plotFileName_retardo= fileNameWithNoExtension_retardo + ".plt";
    std::string plotTitle_retardo="Retardo Medio en función del tamano de cola";
    
    Gnuplot grafico_retardo(plotFileName_retardo);
    grafico_retardo.SetTitle (plotTitle_retardo);
    grafico_retardo.SetLegend ("Tamaño cola servidor (pckts) ","Retardo Medio (ns)");
    std::ofstream fichero_retardo(plotFileName_retardo);

    // --> Modificamos el número de fuentes Generamos una curva para cada gráfico en cada iteración.
    uint32_t num_fuentes_inicial = num_fuentes;

    for (int i = 0; i < num_curvas; i ++){
            
        Gnuplot2dDataset curva_perdidas;
        Gnuplot2dDataset curva_retardo;

        std::string dataTitle_perdidas="Número de fuentes: " + std::to_string(num_fuentes);
        curva_perdidas.SetTitle (dataTitle_perdidas);
        curva_perdidas.SetStyle (Gnuplot2dDataset::LINES_POINTS);

        std::string dataTitle_retardo="Número de fuentes: " + std::to_string(num_fuentes);
        curva_retardo.SetTitle (dataTitle_retardo);
        curva_retardo.SetStyle (Gnuplot2dDataset::LINES_POINTS);

        double tam_cola = tam_cola_inicial;
        
        //Obtenemos puntos para un número de fuentes
        for ( int j = 0; j < num_puntos; j++){
            escenario(num_fuentes, n_max_paq, duracion_simulacion, int_envio_cliente, tam_paq_value, c_transmision, tam_cola,  error, &curva_perdidas, &curva_retardo);
            tam_cola-=(tam_cola/2);
        }
        grafico_perdidas.AddDataset(curva_perdidas);
        grafico_retardo.AddDataset(curva_retardo);

        num_fuentes += (num_fuentes_inicial);
    }
    grafico_perdidas.GenerateOutput(fichero_perdidas);
    fichero_perdidas << "pause -1" << std::endl;
    fichero_perdidas.close();

    grafico_retardo.GenerateOutput(fichero_retardo);
    fichero_retardo << "pause -1" << std::endl;
    fichero_retardo.close();
    */

   escenario(num_fuentes, n_max_paq, duracion_simulacion, int_envio_cliente, tam_paq_value, c_transmision, tam_cola_inicial);

}

void escenario(uint32_t num_fuentes, double n_max_paq, Time duracion_simulacion, Time int_envio_cliente, double tam_paq, DataRateValue c_transmision, double tam_cola)
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
    puente=puent.Puentehelper(c_todos,c_dispositivos, c_transmision);
    

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
        app_c.Get(i)->GetObject<OnOffApplication>()->SetMaxBytes(n_max_paq*tam_paq);
        app_c.Get(i)->SetStopTime(duracion_simulacion);
    }    

   

    //Obtenemos la cola del dispositivo de red de una de las fuentes.
    Ptr<Queue<Packet>> cola_disp_fuente = c_dispositivos.Get(1)->GetObject<CsmaNetDevice>()->GetQueue();

    //control de tráfico.
    Ptr<TrafficControlLayer> tcl = clientes_c.Get(0)->GetObject<TrafficControlLayer> ();
    Ptr<QueueDisc> cola_tcl_fuente = tcl->GetRootQueueDiscOnDevice(c_dispositivos.Get(1));
 
    //Creamos el observador de las colas tcl y transmisión.
    ColaObservador* cola_observador = new ColaObservador(app_c.Get(0)->GetObject<OnOffApplication>(), cola_disp_fuente, cola_tcl_fuente);

    //Valor inicial de la cola
    //cola_fuente->SetMaxSize(QueueSize(ns3::PACKETS, tam_cola));

    //Retardo retardo(app_c, num_fuentes, app_container_sumidero.Get(0)->GetObject<UdpServer>());

    //cola_servidor->ResetStatistics();
    
    NS_LOG_INFO("Arranca Simulación");
    Simulator::Stop(duracion_simulacion);
    Simulator::Run();

    NS_LOG_INFO("Número de fuentes: "<< num_fuentes);
    NS_LOG_INFO("Tamaño de cola del dispositivo: "<< cola_disp_fuente->GetMaxSize().GetValue());
    NS_LOG_INFO("Tamaño de cola de control de tráfico: "<< cola_tcl_fuente->GetMaxSize().GetValue());
    //NS_LOG_INFO("Número de paquetes recibidos por el servidor UDP: " << observador_udp->TotalPaquetes());
    //NS_LOG_INFO("Número de paquetes recibidos por la cola del conmutador: " << cola_servidor->GetTotalReceivedPackets());
    //NS_LOG_INFO("Numero de paquetes descartados por la cola: "<< cola_fuente->GetTotalDroppedPackets());
    //NS_LOG_INFO("Número total de paquetes transmitidos: "<< retardo.TotalPaquetesTx());

    //double porcentaje_paquetes_perdidos = 100*(double)cola_servidor->GetTotalDroppedPackets() / (double)(retardo.TotalPaquetesTx());
    
    //NS_LOG_INFO("Porcentaje de paquetes perdidos: "<< porcentaje_paquetes_perdidos << "%");

    //NS_LOG_INFO("Retardo medio: "<< retardo.RetardoMedio());
    NS_LOG_INFO("Fin simulación");

    //Introducimos los puntos en las curvas que nos pasa el método principal.
    //curva_perdidas->Add(cola_servidor->GetMaxSize().GetValue(), porcentaje_paquetes_perdidos);
    //curva_retardo->Add(cola_servidor->GetMaxSize().GetValue(), retardo.RetardoMedio().GetNanoSeconds());
    
 

}








