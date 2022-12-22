#include "ns3/ptr.h"
#include "ns3/udp-server.h"
#include "ns3/udp-client.h"
#include "ns3/queue-disc.h"
#include "ns3/queue.h"
#include "ns3/onoff-application.h"

using namespace ns3;

class ColaObservador
{
public:
           ColaObservador      (Ptr<OnOffApplication> app_fuente, Ptr<Queue<Packet>> cola_dispo, Ptr<QueueDisc> cola_tcl);
  void     PaqueteTx_fuente (Ptr<const Packet> paquete);
  void Paquetetirado_fuente(Ptr< const QueueDiscItem > item);

private:
  Ptr<Queue<Packet>> m_cola_dispo;
  Ptr<QueueDisc> m_cola_tcl;
};