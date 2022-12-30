/*
    Planificación y simulación de redes 2022/2023.
    
    Práctica 06 

    Autores: Nombre - UVUS 
        Manuel Domínguez Montero - mandommon.
        Alberto Ávila Fernandez - poner aquí !!.

========================================================        
*/
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
                     uint64_t tam_paq, DataRateValue c_transmision, double tam_cola, double tam_tcl,
                     Average<double>* retardo_medio);

int main(int argc, char *argv[])
{
    Time::SetResolution(Time::NS);
  
    uint32_t num_fuentes = 8;  
    uint64_t tam_paq_value = 80;
    std::string tasa_envio_value = "64kbps";
    std::string t_on_value = "350ms";
    std::string t_off_value = "650ms";
    std::string duracion_comunicacion_value = "50s";
    double tam_tcl_ini = 50;
    double tam_cola = 1;
    int num_curvas = 4;
    int num_puntos = 8; 

    // Capacidad de los enlaces
    std::string c_transmision_value = "200kbps";
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
    DataRateValue c_transmision = DataRateValue(c_transmision_value);
    DataRate tasa_envio = DataRate(tasa_envio_value);

    Ptr<ExponentialRandomVariable> t_on  = CreateObject<ExponentialRandomVariable> ();
    t_on->SetAttribute ("Mean", DoubleValue (Time(t_on_value).GetMilliSeconds()));

    Ptr<ExponentialRandomVariable> t_off = CreateObject<ExponentialRandomVariable> ();
    t_off->SetAttribute ("Mean", DoubleValue (Time(t_off_value).GetMilliSeconds()));

    Time duracion_comunicacion = Time(duracion_comunicacion_value);

    Time duracion_simulacion = Time( duracion_comunicacion + Time("100ms") ); //Tiempo prudencial para establecer conexión


    // Gráfica -> Retardo medio
    //===========================================================================================
    std::string fileNameWithNoExtension_retardo = "practica06_retardo";
    std::string graphicsFileName_retardo = fileNameWithNoExtension_retardo + ".png";
    std::string plotFileName_retardo = fileNameWithNoExtension_retardo + ".plt";
    std::string plotTitle_retardo = "R.medio/tam.Cola tcl, v.Tx: "+ c_transmision_value + ", t.paq: " 
                                    + std::to_string(tam_paq_value) +" octetos, duración com: "+ duracion_comunicacion_value;
    
    Gnuplot grafico_retardo(plotFileName_retardo);
    grafico_retardo.SetTitle (plotTitle_retardo);
    grafico_retardo.SetLegend ("Tamaño cola servidor (pckts)"," Retardo medio (ms)");
    std::ofstream fichero_retardo(plotFileName_retardo);


    // --> Bucle principal.
    //============================================================================================
    uint32_t num_fuentes_inicial = num_fuentes;
    double TSTUDENT=2.201;

    for (int i = 0; i < num_curvas; i ++){
            
        Gnuplot2dDataset curva_retardo;

        std::string dataTitle_retardo="Número de fuentes: " + std::to_string(num_fuentes);
        curva_retardo.SetTitle (dataTitle_retardo);
        curva_retardo.SetStyle (Gnuplot2dDataset::LINES_POINTS);
        curva_retardo.SetErrorBars(Gnuplot2dDataset::Y);

        double tam_tcl = tam_tcl_ini;
        
        Average<double> retardo_medio;
        //Obtenemos puntos para un número de fuentes
        for ( int j = 0; j < num_puntos; j++){
           
            for( int z = 0; z < 12; z++){
                escenario(num_fuentes, duracion_simulacion, duracion_comunicacion,
                        tasa_envio, t_on, t_off,
                        tam_paq_value, c_transmision, tam_cola, tam_tcl,
                        &retardo_medio);   
            }
            double error=TSTUDENT*sqrt(retardo_medio.Var()/retardo_medio.Count());
            curva_retardo.Add(tam_tcl, retardo_medio.Avg(), error);
            tam_tcl+=5;
            retardo_medio.Reset();
        }
        grafico_retardo.AddDataset(curva_retardo);

        num_fuentes += (num_fuentes_inicial);
    }
    grafico_retardo.GenerateOutput(fichero_retardo);
    fichero_retardo << "pause -1" << std::endl;
    fichero_retardo.close();

}


void escenario(uint32_t num_fuentes, Time duracion_simulacion, Time duracion_comunicacion,
                DataRate tasa_envio, Ptr<ExponentialRandomVariable> t_on, 
                Ptr<ExponentialRandomVariable> t_off, uint64_t tam_paq, DataRateValue c_transmision,
                 double tam_cola, double tam_tcl, Average<double>* retardo_medio)
{
    
    // --> Nodos y Contenedores.
    //=====================================================================================================
    Ptr<Node> encaminador = CreateObject<Node>();
    Ptr<Node> sumidero = CreateObject<Node>();
    NodeContainer fuentes(num_fuentes);
    
    NodeContainer nodos_red_interna;
    nodos_red_interna.Add(encaminador);
    nodos_red_interna.Add(fuentes);

    NodeContainer nodos_red_externa;
    nodos_red_externa.Add(encaminador);
    nodos_red_externa.Add(sumidero);

    NodeContainer nodos;
    nodos.Add(encaminador);
    nodos.Add(sumidero);
    nodos.Add(fuentes);

    // --> Puente
    //=====================================================================================================
    PuenteHelper h_puente;
    NetDeviceContainer disp_red_interna;

    Ptr<Node> puente = h_puente.Puentehelper(nodos_red_interna, disp_red_interna, c_transmision);

    // --> Encaminador
    //=====================================================================================================
    CsmaHelper h_csma;
    h_csma.SetChannelAttribute("DataRate", c_transmision);

    //-> Nivel de enlace
    NetDeviceContainer disp_red_externa;
    disp_red_externa = h_csma.Install(nodos_red_externa);

    // Nivel de Red.
    //=====================================================================================================
    InternetStackHelper h_pila;
    h_pila.SetIpv6StackInstall(false);
    h_pila.Install(nodos);
   
    // --> Red interna, direccionamiento.
    //=====================================================================================================
    Ipv4AddressHelper h_direcciones_interna ("10.1.0.0", "255.255.0.0");

    Ipv4InterfaceContainer interfaces_red_interna = h_direcciones_interna.Assign (
        disp_red_interna
    );
    
    // --> Red Externa, Encaminador por su interfaz externa y sumidero, direccionamiento.
    //=====================================================================================================
    Ipv4AddressHelper h_direcciones_externa ("10.2.0.0", "255.255.0.0");

    Ipv4InterfaceContainer interfaces_red_externa = h_direcciones_externa.Assign (
        disp_red_externa
    );

    // --> habilitamos encaminamiento.
    //=====================================================================================================
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    // --> Sumidero
    //=====================================================================================================
    uint16_t puerto=20;

    UdpServerHelper sum_udp(puerto);
    ApplicationContainer app_container_sumidero=sum_udp.Install(sumidero); 

    // --> tam de cola de transmisión.
     //=====================================================================================================
    for( uint32_t i = 0; i < num_fuentes; i++ ){
        fuentes.Get(i)->GetDevice(0)->GetObject<CsmaNetDevice>()
            ->GetQueue()->SetMaxSize(QueueSize( ns3::PACKETS, tam_cola));
    }
    for( uint32_t i = 0; i < 2; i++ ){
        encaminador->GetDevice(i)->GetObject<CsmaNetDevice>()
            ->GetQueue()->SetMaxSize(QueueSize( ns3::PACKETS, tam_cola));
    }
    sumidero->GetDevice(0)->GetObject<CsmaNetDevice>()
        ->GetQueue()->SetMaxSize(QueueSize( ns3::PACKETS, tam_cola));

     

    //--> OnOfApplication fuentes
    //=====================================================================================================
    InetSocketAddress direcciones(interfaces_red_externa.GetAddress(1,0),puerto);
    OnOffHelper onoff("ns3::UdpSocketFactory",direcciones);
    
    onoff.SetAttribute("OnTime", PointerValue(t_on));
    onoff.SetAttribute("OffTime", PointerValue(t_off));
    onoff.SetAttribute("DataRate", DataRateValue(tasa_envio));
    onoff.SetAttribute("PacketSize", UintegerValue(tam_paq));
    onoff.SetAttribute("StopTime", TimeValue(duracion_comunicacion));

    ApplicationContainer app_c;

    app_c.Add(onoff.Install(fuentes));
    
    //-->Trazas y control de tráfico.
    //=====================================================================================================
    Ptr<TrafficControlLayer> tcl = encaminador->GetObject<TrafficControlLayer> ();
    Ptr<QueueDisc> cola_tcl = tcl->GetRootQueueDiscOnDevice( encaminador->GetDevice(1) );
    cola_tcl->SetMaxSize(QueueSize( ns3::PACKETS, tam_tcl));

    Ptr<Queue<Packet>> cola_router = encaminador->GetDevice(1)->GetObject<CsmaNetDevice>()->GetQueue(); // Cola tx del router.    

    Retardo retardo(app_c, num_fuentes, app_container_sumidero.Get(0)->GetObject<UdpServer>());
    ColaObservador cola_observador(app_c, num_fuentes, cola_router, cola_tcl);

    std::cout <<"Nueva Simulación.\n";
    std::cout <<"===========================================================================================\n";
  
    NS_LOG_INFO("Número de fuentes: "<< num_fuentes);
    NS_LOG_INFO("Régimen Binario de las fuentes : " << tasa_envio);
    NS_LOG_INFO("Tamaño de cola de dispositivos: "<< tam_cola);
    NS_LOG_INFO("Tamaño de cola de control de tráfico: "<< cola_tcl->GetMaxSize());
    NS_LOG_INFO("Tamaño de paquete VoIP: "<< std::to_string(tam_paq));
    NS_LOG_INFO("Duración de la comunicación VoIP: "<< duracion_comunicacion);
    std::cout <<"\n";
 
    NS_LOG_INFO("Arranca Simulación");
    Simulator::Stop(duracion_simulacion);
    Simulator::Run();
    NS_LOG_INFO("Fin simulación");

    double retardo_salida;

    if( retardo.TotalPaquetesRx() == 0 ){
        retardo_salida = 0;
    }   
    else{
        retardo_salida = retardo.RetardoMedio().GetMilliSeconds();
    }
    
    std::cout <<"\nSumario: \n------------------\n";
    NS_LOG_INFO("Número de paquetes transmitidos: "<< cola_observador.TotalPaquetesTx());
    NS_LOG_INFO("Número de paquetes recibidos: "<< retardo.TotalPaquetesRx());
    NS_LOG_INFO("Retardo medio (ms): "<< std::to_string(retardo_salida));
    std::cout <<"===========================================================================================\n\n";
    
    //Introducimos los puntos en la curva.
    //=======================================================================================================
   
    retardo_medio->Update (retardo_salida);
    
    Simulator::Destroy();
}








