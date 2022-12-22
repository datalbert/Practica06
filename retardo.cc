#include "ns3/object-base.h"
#include "ns3/log.h"
#include "retardo.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/onoff-application.h"
#include "ns3/application-container.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Retardo");

TimestampTag tag;
Retardo::Retardo (ApplicationContainer c_app_fuentes, int num_fuentes, Ptr<UdpServer> receptor){

  m_cuenta = 0;
  for (int i = 0; i < num_fuentes; i++)
  {
    c_app_fuentes.Get(i)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx",
					                            MakeCallback(&Retardo::PaqueteTransmitido,
                                                    this));
  }
  receptor->TraceConnectWithoutContext ("Rx",
                                        MakeCallback(&Retardo::PaqueteRecibido,
                                                     this)); 
  
}


void
Retardo::PaqueteTransmitido(Ptr<const Packet> paquete){
  
  //-->Etiquetamos el paquete
  //TimestampTag tag;
  tag.SetTimestamp(Simulator::Now());
  paquete->AddPacketTag(tag);  

  Time tiempo = tag.GetTimestamp();
  NS_LOG_INFO(tiempo);
}


void
Retardo::PaqueteRecibido(Ptr<const Packet> paquete)
{
  NS_LOG_INFO (paquete <<" Paquete rx en:" << Simulator::Now().GetSeconds());
  
  //Obtenemos la etiqueta del paquete y el tiempo de envío
  //TimestampTag tag;
  paquete->PeekPacketTag(tag);

  Time tiempo_tx = tag.GetTimestamp();
  
  media_retardo.Update((
      Simulator::Now() - tiempo_tx 
    ).GetDouble());
  Time retardo=Simulator::Now() - tiempo_tx;
    NS_LOG_INFO ("Transmitido: "<< tiempo_tx);
    NS_LOG_DEBUG("RETARDO:" << retardo);
}


Time
Retardo::RetardoMedio ()
{
  return NanoSeconds(media_retardo.Mean());
}

double
Retardo::TotalPaquetesTx(){
  return m_cuenta;
}