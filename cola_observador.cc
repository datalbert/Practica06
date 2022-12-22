#include "ns3/object-base.h"
#include "ns3/log.h"
#include "cola_observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ColaObservador");


ColaObservador::ColaObservador (Ptr<OnOffApplication> app_fuente, Ptr<Queue<Packet>> cola_dispo, Ptr<QueueDisc> cola_tcl)
{
  app_fuente->TraceConnectWithoutContext ("Tx",
                                        MakeCallback(&ColaObservador::PaqueteTx_fuente,
                                                     this));  

  m_cola_dispo = cola_dispo;
  m_cola_tcl = cola_tcl;
  m_cuenta = 0;
}


void
ColaObservador::PaqueteTx_fuente (Ptr<const Packet> paquete)
{
  m_cuenta++;

  m_media_tx.Update((Simulator::Now() - m_prev_time).GetSeconds());
  m_prev_time = Simulator::Now();

  NS_LOG_DEBUG (paquete << "Paquete Tx:\n"<<
                "=================================================================");
  NS_LOG_DEBUG (paquete << "Número de paquetes en la cola del dispositivo: " << m_cola_dispo->GetNPackets());
  NS_LOG_DEBUG (paquete << "Número de paquetes en la cola tcl: " << m_cola_tcl->GetNPackets());
}


double
ColaObservador::GetMediaIntervaloTx (void)
{
  return m_media_tx.Avg();
}


double
ColaObservador::GetNPaquetesTx (void)
{
  return m_cuenta;
}
