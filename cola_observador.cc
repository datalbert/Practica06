#include "ns3/object-base.h"
#include "ns3/log.h"
#include "cola_observador.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ColaObservador");


ColaObservador::ColaObservador (Ptr<OnOffApplication> app_fuente, Ptr<Queue<Packet>> cola_dispo, Ptr<QueueDisc> cola_tcl)
{
  app_fuente->TraceConnectWithoutContext ("Tx",
                                        MakeCallback(&ColaObservador::PaqueteTx_fuente,
                                                     this));
  cola_tcl->TraceConnectWithoutContext ("Drop",
                                        MakeCallback(&ColaObservador::Paquetetirado_fuente,
                                                     this));

  m_cola_dispo = cola_dispo;
  m_cola_tcl = cola_tcl;
}


void
ColaObservador::PaqueteTx_fuente (Ptr<const Packet> paquete)
{
  
    NS_LOG_INFO (paquete << "Número de paquetes en la cola del dispositivo: " << m_cola_dispo->GetNPackets());
    NS_LOG_INFO (paquete << "Número de paquetes en la cola tcl: " << m_cola_tcl->GetNPackets());
  

}
void
ColaObservador::Paquetetirado_fuente (Ptr< const QueueDiscItem> item) {
  NS_LOG_DEBUG ("Paquete tirado por la cola tcl" << Simulator::Now());

}
