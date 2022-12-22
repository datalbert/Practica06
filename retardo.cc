#include "ns3/object-base.h"
#include "ns3/log.h"
#include "retardo.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/onoff-application.h"
#include "ns3/application-container.h"
#include "ns3/packet.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Retardo");

TimestampTag etiqueta_tiempo_paq;
Retardo::Retardo (ApplicationContainer c_app_fuentes, Ptr<UdpServer> receptor){

  m_cuenta = 0;
  
  c_app_fuentes.Get(0)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx",
					                            MakeCallback(&Retardo::PaqueteTransmitido,
                                                    this));
  
  receptor->TraceConnectWithoutContext ("Rx",
                                        MakeCallback(&Retardo::PaqueteRecibido,
                                                     this)); 
  
}


void
Retardo::PaqueteTransmitido(Ptr<const Packet> paquete){
  m_tiempo_tx = Simulator::Now();
  etiqueta_tiempo_paq.SetTimestamp(m_tiempo_tx);
  paquete->AddPacketTag(etiqueta_tiempo_paq);
  NS_LOG_INFO ("Paquete tx en:" << m_tiempo_tx.GetSeconds());
  
  m_cuenta++;
}


void
Retardo::PaqueteRecibido(Ptr<const Packet> paquete)
{
  
  NS_LOG_FUNCTION (paquete);
  //m_tiempo_rx = Simulator::Now();
  NS_LOG_DEBUG("entra aqui");
  paquete->PeekPacketTag(etiqueta_tiempo_paq);
  m_tiempo_rx=etiqueta_tiempo_paq.GetTimestamp();
  NS_LOG_DEBUG("RETARDO MEDIO"<<(Simulator::Now() - m_tiempo_rx).GetDouble());
  media_retardo.Update((Simulator::Now() - m_tiempo_rx).GetDouble());
  

}


Time
Retardo::RetardoMedio ()
{
  return Seconds(media_retardo.Avg());
}

double
Retardo::TotalPaquetesTx(){
  return m_cuenta;
}