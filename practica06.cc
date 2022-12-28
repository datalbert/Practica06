
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

void escenario(uint32_t num_fuentes, Time duracion_simulacion, Time duracion_comunicacion,
                     DataRate tasa_envio, Ptr<ExponentialRandomVariable> t_on, Ptr<ExponentialRandomVariable> t_off,
                     uint64_t tam_paq, DataRate c_transmision, double tam_cola, double tam_tcl,
                     Average<double>* retardo_media);

int main(int argc, char *argv[])
{
    Time::SetResolution(Time::NS);
  
    uint32_t num_fuentes= 5;  
    uint64_t tam_paq_value = 8192;
    std::string tasa_envio_value = "96kbps";
    std::string t_on_value = "350ms";
    std::string t_off_value = "650ms";
    std::string duracion_comunicacion_value = "300s";
    double tam_tcl_ini = 2;
    double tam_cola = 1;
    int num_curvas = 4;
    int num_puntos = 8; 

    //Capacidad de los enlaces
    std::string c_transmision_value = "100Kb/s";
    //-----------------------------------------------------------

    // Configuracion linea de comandos
    CommandLine cmd(__FILE__);

    cmd.AddValue("num_curvas", "Veces que aumentará el número de fuentes", num_curvas);
    cmd.AddValue("num_puntos", "Veces que se decrementará la cola de paquetes", num_puntos);
    cmd.AddValue("num_fuentes", "Numeros de fuentes del escenario a crear", num_fuentes);
    cmd.AddValue("tam_paq", "Tamano del paquete UDP", tam_paq_value);
    cmd.AddValue("tam_cola", "Tamano de cola de dispositivos en número de paquetes", tam_cola);
    cmd.AddValue("tam_tcl", "Tamano de cola tcl en número de paquetes", tam_tcl_ini);
    cmd.AddValue("c_transmision", "Capacidad de transmision de los enlaces del conmutador", c_transmision_value);
    cmd.AddValue("t_on", "Duración media estado ON", t_on_value);
    cmd.AddValue("t_off", "Duración media estado OFF", t_off_value);
    cmd.AddValue("tasa_envio", "Tasa de envio de las fuentes", tasa_envio_value);
    cmd.AddValue("duracion_comunicacion", "Duración de la comunicación", duracion_comunicacion_value);

    cmd.Parse(argc, argv);

    // Conversión de parámetros a objetos ns3
    //=========================================================================================
    DataRate c_transmision = DataRate("100kb/s");
    DataRate tasa_envio = DataRate(tasa_envio_value);

    Ptr<ExponentialRandomVariable> t_on  = CreateObject<ExponentialRandomVariable> ();
    t_on->SetAttribute ("Mean", DoubleValue (Time(t_on_value).GetSeconds ()));

    Ptr<ExponentialRandomVariable> t_off = CreateObject<ExponentialRandomVariable> ();
    t_off->SetAttribute ("Mean", DoubleValue (Time(t_off_value).GetSeconds ()));

    Time duracion_comunicacion = Time(duracion_comunicacion_value);

    //Duración de la simulación acorde al número máximo de paquetes a enviar.
    Time duracion_simulacion = Time( duracion_comunicacion + Time("100ms") ); //Tiempo prudencial para establecer conexión


    //Gráfica -> Retardo medio (ms)
    //===========================================================================================
    std::string fileNameWithNoExtension_retardo = "practica06_retardo";
    std::string graphicsFileName_retardo = fileNameWithNoExtension_retardo + ".png";
    std::string plotFileName_retardo = fileNameWithNoExtension_retardo + ".plt";
    std::string plotTitle_retardo="Retardo medio (ms) en funcion del tamaño de cola tcl";
    
    Gnuplot grafico_retardo(plotFileName_retardo);
    grafico_retardo.SetTitle (plotTitle_retardo);
    grafico_retardo.SetLegend ("Tamaño cola servidor (pckts)"," Retardo medio (ms)");
    std::ofstream fichero_retardo(plotFileName_retardo);


    //--> Bucle principal.
    //============================================================================================
    uint32_t num_fuentes_inicial = num_fuentes;
    double TSTUDENT=2.2010;
    for (int i = 0; i < num_curvas; i ++){
            
        Gnuplot2dDataset curva_retardo;

        std::string dataTitle_retardo="Número de fuentes: " + std::to_string(num_fuentes);
        curva_retardo.SetTitle (dataTitle_retardo);
        curva_retardo.SetStyle (Gnuplot2dDataset::LINES_POINTS);
        curva_retardo.SetErrorBars(Gnuplot2dDataset::Y);
        double tam_tcl = tam_tcl_ini;
        
        //Obtenemos puntos para un número de fuentes
        Average<double> retardo_media;
        for ( int j = 0; j < num_puntos; j++){
            for (int z=0; z<12;z++){
                escenario(num_fuentes, duracion_simulacion, duracion_comunicacion,
                        tasa_envio, t_on, t_off,
                        tam_paq_value, c_transmision, tam_cola, tam_tcl,
                        &retardo_media);
                
            }
            NS_LOG_DEBUG("Retardo para la curva "<< i << " el punto " << j << " con tam cola; "<< tam_tcl <<" es igual " << retardo_media.Avg());
            double error=TSTUDENT*sqrt(retardo_media.Var()/retardo_media.Count());
            curva_retardo.Add(tam_tcl, retardo_media.Avg(),error);
            tam_tcl+=5;
            retardo_media.Reset();
        }
        NS_LOG_DEBUG("------------------");

        grafico_retardo.AddDataset(curva_retardo);

        num_fuentes += (num_fuentes_inicial);
    }
    grafico_retardo.GenerateOutput(fichero_retardo);
    fichero_retardo << "pause -1" << std::endl;
    fichero_retardo.close();

}


void escenario(uint32_t num_fuentes, Time duracion_simulacion, Time duracion_comunicacion,
                DataRate tasa_envio, Ptr<ExponentialRandomVariable> t_on, 
                Ptr<ExponentialRandomVariable> t_off, uint64_t tam_paq, DataRate c_transmision,
                 double tam_cola, double tam_tcl, Average<double>* retardo_media)
{

    Simulator::Destroy();
    
    NodeContainer nodos;

    Ptr<Node> fuente = CreateObject<Node>();
    Ptr<Node> sumidero=CreateObject<Node>();

    nodos.Add(sumidero);
    nodos.Add(fuente);
    
    InternetStackHelper h_pila;
    h_pila.SetIpv6StackInstall(false);
    h_pila.Install(nodos);

    NetDeviceContainer c_dispositivos;
    
    PuenteHelper h_puente;
    Ptr<Node> puente = h_puente.Puentehelper(nodos, c_dispositivos, c_transmision);

    Ipv4AddressHelper h_direcciones ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer c_interfaces = h_direcciones.Assign (c_dispositivos);
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    
    uint16_t puerto=20;
    InetSocketAddress direcciones(c_interfaces.GetAddress(0,0),puerto);
    

    //Configuracion sumidero
    //====================================================================================================
    UdpServerHelper sum_udp(puerto);
    ApplicationContainer app_container_sumidero=sum_udp.Install(sumidero); 
    
    //--> OnOfApplication
    //=====================================================================================================
    OnOffHelper onoff("ns3::UdpSocketFactory",direcciones);
    
    onoff.SetAttribute("OnTime", PointerValue(t_on));
    onoff.SetAttribute("OffTime", PointerValue(t_off));
    //onoff.SetAttribute("DataRate", DataRateValue(tasa_envio));
    //onoff.SetAttribute("PacketSize", UintegerValue(tam_paq));
    
    onoff.SetConstantRate(tasa_envio);
    onoff.SetAttribute("StopTime", TimeValue(duracion_comunicacion));

    ApplicationContainer app_c;

    for (uint32_t i = 0; i <= num_fuentes; i++){
        app_c.Add(onoff.Install(nodos.Get(1)));
    }
        
    //-->Trazas y control de tráfico.
    //=====================================================================================================
    Ptr<Queue<Packet>> cola_disp = c_dispositivos.Get(1)->GetObject<CsmaNetDevice>()->GetQueue();
    cola_disp->SetMaxSize(QueueSize( ns3::PACKETS, tam_cola));

    Ptr<TrafficControlLayer> tcl = fuente->GetObject<TrafficControlLayer> ();
    Ptr<QueueDisc> cola_tcl = tcl->GetRootQueueDiscOnDevice(c_dispositivos.Get(1));
    cola_tcl->SetMaxSize(QueueSize( ns3::PACKETS, tam_tcl));
 
    ColaObservador* observador = new ColaObservador(app_c.Get(0)->GetObject<OnOffApplication>(), cola_disp, cola_tcl);
    Retardo retardo(app_c, num_fuentes, app_container_sumidero.Get(0)->GetObject<UdpServer>());
    

    
  
    NS_LOG_INFO("Número de fuentes: "<< num_fuentes);
    NS_LOG_INFO("Régimen Binario de las fuentes : " << tasa_envio);
    NS_LOG_INFO("Tamaño de cola del dispositivo: "<< tam_cola);
    NS_LOG_INFO("Tamaño de colas de control de tráfico: "<< tam_tcl);
    NS_LOG_INFO("Duracion de la comunicación VoIP: "<< duracion_comunicacion);
 
    NS_LOG_INFO("Arranca Simulación");
    Simulator::Stop(duracion_simulacion);
    Simulator::Run();
    NS_LOG_INFO("Fin simulación");
    

    NS_LOG_INFO("OnOffApplication--> Comprobación media T.Total envío de un paquete: "<< observador->GetMediaIntervaloTx()<<" (s)");
    NS_LOG_INFO("Número de paquetes transmitidos por una fuente: "<< observador->GetNPaquetesTx());
    NS_LOG_INFO("Retardo medio: "<< retardo.RetardoMedio());
   

    //Introducimos los puntos en la curva.
    //=======================================================================================================
    retardo_media->Update(retardo.RetardoMedio().GetDouble());
    
    
}








