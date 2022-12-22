
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

double escenario(uint32_t num_fuentes, /*double n_max_paq,*/ Time duracion_simulacion, /*Time int_envio_cliente,*/
                    Time duracion_comunicacion,
                     DataRate tasa_envio, Ptr<ExponentialRandomVariable> t_on, Ptr<ExponentialRandomVariable> t_off,
                     uint64_t tam_paq, DataRateValue c_transmision, double tam_cola);


Gnuplot crea_grafico(std::string graphicsFileName,uint32_t num_fuentes,uint32_t num_curvas,int num_puntos, Time duracion_simulacion,Time duracion_comunicacion,DataRate tasa_envio, Ptr<ExponentialRandomVariable> t_on, Ptr<ExponentialRandomVariable> t_off,
                     uint64_t tam_paq, DataRateValue c_transmision, double tam_cola);
int main(int argc, char *argv[])
{
    Time::SetResolution(Time::NS);
  
    uint32_t num_fuentes= 8;  // Último dígito del número de grupo. 
    //std::string int_envio_cliente_value = "1ms";  //Por defecto es el intervalo mínimo, cercano al teórico al que no se pierden paquetes.
    uint64_t tam_paq_value = 8192;
    std::string tasa_envio_value = "64kbps";
    std::string t_on_value = "350ms";
    std::string t_off_value = "650ms";
    std::string duracion_comunicacion_value = "50s";

    double tam_cola_inicial = 1;
    

    //Capacidad de los enlaces
    std::string c_transmision_value = "100kbps";
    //-----------------------------------------------------------

    //configuracion de los parametros de la grafica

    uint32_t num_curvas=4;
    int num_puntos=8;

    // Configuracion linea de comandos
    CommandLine cmd(__FILE__);

    
    
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

    //creacion del grafico llamando al medodo gnuplot crea_grafico

    std::string fileNameWithNoExtension = "captura03";
    std::string graphicsFileName        = fileNameWithNoExtension + ".png";
    std::string plotFileName            = fileNameWithNoExtension + ".plt";
    std::string plotTitle               = "2-D Plot";
    std::string dataTitle               = "2-D Data";

    Gnuplot grafico=crea_grafico(graphicsFileName,num_fuentes, num_curvas,num_puntos,duracion_simulacion, duracion_comunicacion,tasa_envio, t_on, t_off,tam_paq_value, c_transmision, tam_cola_inicial);

    std::ofstream plotFile (plotFileName.c_str());

    grafico.GenerateOutput (plotFile);

    plotFile.close ();

   
}

double 
escenario(uint32_t num_fuentes, /*double n_max_paq,*/ Time duracion_simulacion, 
                Time duracion_comunicacion,
                /*Time int_envio_cliente,*/ DataRate tasa_envio,
                Ptr<ExponentialRandomVariable> t_on, Ptr<ExponentialRandomVariable> t_off,
                uint64_t tam_paq, DataRateValue c_transmision, double tam_cola)
{

    //--> Nodos
    //=====================================================================================================
    Simulator::Destroy();
    Ptr<Node> sumidero=CreateObject<Node>();
    Ptr<Node> fuente=CreateObject<Node>();
    NodeContainer c_todos;
    c_todos.Add(fuente);
    c_todos.Add(sumidero);

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
    ApplicationContainer app_c;

    for (int i=0 ;i<num_fuentes;i++){
       
        app_c.Add(onoff.Install(fuente));
    }
    
    //-->Control de tráfico.
    //=====================================================================================================
    Ptr<Queue<Packet>> cola_disp_fuente = c_dispositivos.Get(0)->GetObject<CsmaNetDevice>()->GetQueue();

    Ptr<TrafficControlLayer> tcl = c_todos.Get(0)->GetObject<TrafficControlLayer> ();
    Ptr<QueueDisc> cola_tcl_fuente = tcl->GetRootQueueDiscOnDevice(c_dispositivos.Get(0));
    cola_tcl_fuente->SetMaxSize(QueueSize(ns3::PACKETS, tam_cola));
 
    ColaObservador* observador_onoff = new ColaObservador(app_c.Get(0)->GetObject<OnOffApplication>(), cola_disp_fuente, cola_tcl_fuente);

    Retardo retardo(app_c,app_container_sumidero.Get(0)->GetObject<UdpServer>());
    

    
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
    
  
    NS_LOG_INFO("Fin simulación");
    return retardo.RetardoMedio().GetDouble();

}

Gnuplot crea_grafico(std::string graphicsFileName, uint32_t num_fuentes,uint32_t num_curvas, int num_puntos, Time duracion_simulacion, Time duracion_comunicacion, DataRate tasa_envio, Ptr<ExponentialRandomVariable> t_on, Ptr<ExponentialRandomVariable> t_off, uint64_t tam_paq, DataRateValue c_transmision, double tam_cola)
{
     Gnuplot plot(graphicsFileName);
    //crear el grafico
    
    int num_fuentes_inicial=num_fuentes;
    int tam_cola_inicial=tam_cola;
    for (int i=0;i<num_curvas;i++){
        Gnuplot2dDataset curva_retardo;
        std::string dataTitle_retardo="Número de fuentes: " + std::to_string(num_fuentes);
        curva_retardo.SetTitle (dataTitle_retardo);
        curva_retardo.SetStyle (Gnuplot2dDataset::LINES_POINTS);

        for (int j=0;j<num_puntos;j++){
            
            //llamar al escenario para que nos devuelva el retardo medio incrementando num_fuente+=num_fuente
            double retardo_medio=escenario(num_fuentes, duracion_simulacion, duracion_comunicacion,
                    tasa_envio, t_on, t_off,
                     tam_paq, c_transmision, tam_cola);
            NS_LOG_DEBUG("TAM COLA" << tam_cola);
            curva_retardo.Add(tam_cola,retardo_medio);
            tam_cola+=5;
            
        }
        num_fuentes+=num_fuentes_inicial;
        tam_cola=tam_cola_inicial;
        plot.AddDataset (curva_retardo);
    }
    return plot;
    
}




