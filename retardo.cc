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
  
  //-->Etiquetamos el paquete
  NS_LOG_DEBUG("Paquete: " << paquete << ", tx en: " << Simulator::Now());
  TimestampTag tag;
  tag.SetTimestamp(Simulator::Now());
  paquete->AddPacketTag(tag);  
}


void
Retardo::PaqueteRecibido(Ptr<const Packet> paquete)
{
  NS_LOG_DEBUG ("Paquete: " << paquete << ", rx en: " << Simulator::Now());
  
  //Obtenemos la etiqueta del paquete y el tiempo de envío
  TimestampTag tag;

  if(paquete->PeekPacketTag(tag)){

    Time tiempo_tx = tag.GetTimestamp();
  
    media_retardo.Update((
      Simulator::Now() - tiempo_tx 
    ).GetDouble());

  }
  //Si llega un paquete sin etiquetar no alteramos el retardo.
  m_cuenta++;

}


Time
Retardo::RetardoMedio ()
{
  return Time(media_retardo.Avg());
}

double
Retardo::TotalPaquetesRx(){
  return m_cuenta;
}