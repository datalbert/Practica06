#include "ns3/ptr.h"
#include "ns3/udp-client.h"
#include "ns3/queue-disc.h"
#include "ns3/queue.h"
#include "ns3/onoff-application.h"

#include "ns3/application-container.h"

using namespace ns3;

class ColaObservador
{
public:
           ColaObservador (ApplicationContainer c_app_fuentes, int num_fuentes, Ptr<Queue<Packet>> cola_dispo, Ptr<QueueDisc> cola_tcl);
  void     PaqueteTransmitido (Ptr<const Packet> paquete);
  double   TotalPaquetesTx (void);

private:
  Ptr<Queue<Packet>> m_cola_dispo;
  Ptr<QueueDisc> m_cola_tcl;
  double m_cuenta;
};