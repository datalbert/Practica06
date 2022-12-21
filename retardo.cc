#include "ns3/object-base.h"
#include "ns3/log.h"
#include "retardo.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/onoff-application.h"
#include "ns3/application-container.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Retardo");


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
  
  m_tiempo_tx = Simulator::Now();
  NS_LOG_INFO ("Paquete tx en:" << m_tiempo_tx.GetSeconds());
  
  m_cuenta++;
}


void
Retardo::PaqueteRecibido(Ptr<const Packet> paquete)
{
  
  NS_LOG_FUNCTION (paquete);
  //m_tiempo_rx = Simulator::Now();
  
  media_retardo.Update((Simulator::Now() - m_tiempo_tx).GetSeconds());

  NS_LOG_INFO ("Paquete rx en:" << m_tiempo_rx.GetSeconds());

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