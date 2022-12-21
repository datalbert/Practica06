
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

#include "ns3/random-variable-stream.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Practica06");

void escenario(uint32_t num_fuentes, /*double n_max_paq,*/ Time duracion_simulacion, /*Time int_envio_cliente,*/
                    Time duracion_comunicacion,
                     DataRate tasa_envio, Ptr<ExponentialRandomVariable> t_on, Ptr<ExponentialRandomVariable> t_off,
                     uint64_t tam_paq, DataRateValue c_transmision, double tam_cola);

int main(int argc, char *argv[])
{
    Time::SetResolution(Time::NS);
  
    uint32_t num_fuentes= 2;  // Último dígito del número de grupo. 
    //std::string int_envio_cliente_value = "1ms";  //Por defecto es el intervalo mínimo, cercano al teórico al que no se pierden paquetes.
    uint64_t tam_paq_value = 8192;
    std::string tasa_envio_value = "32kbps";
    std::string t_on_value = "350ms";
    std::string t_off_value = "650ms";
    std::string duracion_comunicacion_value = "50s";

    double tam_cola_inicial = 50;
    //int num_curvas = 4;
    //int num_puntos = 5; // Numero de veces que se decrementará la cola del conmutador.
    //double error = 2;
    //double n_max_paq = ( 400 + 100*( 13 % 4 ) ); //13 -> número de grupo

    //Capacidad de los enlaces
    std::string c_transmision_value = "1Mbps";
    //-----------------------------------------------------------

    // Configuracion linea de comandos
    CommandLine cmd(__FILE__);

    //cmd.AddValue("num_curvas", "Veces que aumentará el número de fuentes", num_curvas);
    //cmd.AddValue("num_puntos", "Veces que se decrementará la cola de paquetes", num_puntos);
    //cmd.AddValue("int_envio_cliente", "Intervalo de envio de paquetes del cliente", int_envio_cliente_value);
    //cmd.AddValue("error", "Error en las gráficas", error);
    //cmd.AddValue("n_max_paq", "Número máximo de paquetes a enviar", n_max_paq);
    
    cmd.AddValue("num_fuentes", "Numeros de fuentes del escenario a crear", num_fuentes);
    cmd.AddValue("tam_paq", "Tamano del paquete UDP", tam_paq_value);
    cmd.AddValue("tam_cola", "Tamano de cola en número de paquetes", tam_cola_inicial);
    cmd.AddValue("c_transmision", "Capacidad de transmision de los enlaces del conmutador", c_transmision_value);
    cmd.AddValue("t_on", "Duración media estado ON", t_on_value);
    cmd.AddValue("t_off", "Duración media estado OFF", t_off_value);
    cmd.AddValue("tasa_envio", "Tasa de envio de las fuentes", tasa_envio_value);
    cmd.AddValue("duracion_comunicacion", "Duración de la comunicación", duracion_comunicacion_value);

    cmd.Parse(argc, argv);

    DataRateValue c_transmision = DataRateValue(c_transmision_value);
    //Time int_envio_cliente = Time(int_envio_cliente_value);

    DataRate tasa_envio = DataRate(tasa_envio_value);

    Ptr<ExponentialRandomVariable> t_on  = CreateObject<ExponentialRandomVariable> ();
    t_on->SetAttribute ("Mean", DoubleValue (Time(t_on_value).GetSeconds ()));

    Ptr<ExponentialRandomVariable> t_off = CreateObject<ExponentialRandomVariable> ();
    t_off->SetAttribute ("Mean", DoubleValue (Time(t_off_value).GetSeconds ()));

    Time duracion_comunicacion = Time(duracion_comunicacion_value);

    //Duración de la simulación acorde al número máximo de paquetes a enviar.
    Time duracion_simulacion = Time( duracion_comunicacion + Time("100ms") ); //Tiempo prudencial para establecer conexión

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

   escenario(num_fuentes, duracion_simulacion, duracion_comunicacion,
                    tasa_envio, t_on, t_off,
                     tam_paq_value, c_transmision, tam_cola_inicial);

}

void escenario(uint32_t num_fuentes, /*double n_max_paq,*/ Time duracion_simulacion, 
                Time duracion_comunicacion,
                /*Time int_envio_cliente,*/ DataRate tasa_envio,
                Ptr<ExponentialRandomVariable> t_on, Ptr<ExponentialRandomVariable> t_off,
                uint64_t tam_paq, DataRateValue c_transmision, double tam_cola)
{

    //--> Nodos
    //=====================================================================================================
    NodeContainer clientes_c(num_fuentes);
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
    
    InetSocketAddress direcciones(c_interfaces.GetAddress(0,0),puerto);
    
    //--> OnOfApplication
    //=====================================================================================================
    OnOffHelper onoff("ns3::UdpSocketFactory",direcciones);
    
    onoff.SetAttribute("OnTime", PointerValue(t_on));
    onoff.SetAttribute("OffTime", PointerValue(t_off));
    onoff.SetAttribute("DataRate", DataRateValue(tasa_envio));
    onoff.SetAttribute("PacketSize", UintegerValue(tam_paq));
    onoff.SetAttribute("StopTime", TimeValue(duracion_comunicacion));

    ApplicationContainer app_c=onoff.Install(clientes_c);
    
    //-->Control de tráfico.
    //=====================================================================================================
    Ptr<Queue<Packet>> cola_disp_fuente = c_dispositivos.Get(1)->GetObject<CsmaNetDevice>()->GetQueue();

    Ptr<TrafficControlLayer> tcl = clientes_c.Get(0)->GetObject<TrafficControlLayer> ();
    Ptr<QueueDisc> cola_tcl_fuente = tcl->GetRootQueueDiscOnDevice(c_dispositivos.Get(1));
    cola_tcl_fuente->SetMaxSize(QueueSize("10p"));
 
    ColaObservador* observador_onoff = new ColaObservador(app_c.Get(0)->GetObject<OnOffApplication>(), cola_disp_fuente, cola_tcl_fuente);

    //Valor inicial de la cola
    //cola_fuente->SetMaxSize(QueueSize(ns3::PACKETS, tam_cola));
    //Retardo retardo(app_c, num_fuentes, app_container_sumidero.Get(0)->GetObject<UdpServer>());
    //cola_servidor->ResetStatistics();
    
    NS_LOG_INFO("Arranca Simulación");
    Simulator::Stop(duracion_simulacion);
    Simulator::Run();

    NS_LOG_INFO("Número de fuentes: "<< num_fuentes);
    NS_LOG_INFO("Régimen Binario de las fuentes : " << tasa_envio);
    NS_LOG_INFO("Tamaño de cola del dispositivo: "<< cola_disp_fuente->GetMaxSize().GetValue());
    NS_LOG_INFO("Tamaño de cola de control de tráfico: "<< cola_tcl_fuente->GetMaxSize().GetValue());
    NS_LOG_INFO("Duracion de la comunicación VoIP: "<< duracion_comunicacion);
    NS_LOG_INFO("OnOffApplication-> Comprobación media T.Total envío de un paquete: "<< observador_onoff->GetMediaIntervaloTx()<<" (s)");
    NS_LOG_INFO("Número de paquetes transmitidos por una fuente: "<< observador_onoff->GetNPaquetesTx());
    

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








