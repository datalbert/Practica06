#include "ns3/object-base.h"
#include "ns3/log.h"
#include "cola_observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ColaObservador");


ColaObservador::ColaObservador (ApplicationContainer c_app_fuentes, int num_fuentes, Ptr<Queue<Packet>> cola_dispo, Ptr<QueueDisc> cola_tcl){

  m_cuenta = 0;
  m_cola_dispo = cola_dispo;
  m_cola_tcl = cola_tcl;
  
  for (int i = 0; i < num_fuentes; i++)
  {
    c_app_fuentes.Get(i)->GetObject<OnOffApplication>()->TraceConnectWithoutContext ("Tx",
					                            MakeCallback(&ColaObservador::PaqueteTransmitido,
                                                    this));
  }  
}


void
ColaObservador::PaqueteTransmitido (Ptr<const Packet> paquete)
{
  m_cuenta++;

  NS_LOG_DEBUG (paquete << "Paquete Tx:\n"<<
                "=================================================================");
  NS_LOG_DEBUG ("Número de paquetes en la cola del dispositivo: " << m_cola_dispo->GetNPackets());
  NS_LOG_DEBUG ("Número de paquetes en la cola tcl: " << m_cola_tcl->GetNPackets());
}


double
ColaObservador::TotalPaquetesTx (void)
{
  return m_cuenta;
}
