#include "ns3/ptr.h"
#include "ns3/udp-client.h"
#include "ns3/queue-disc.h"
#include "ns3/queue.h"
#include "ns3/onoff-application.h"
#include "ns3/average.h"

using namespace ns3;

class ColaObservador
{
public:
           ColaObservador      (Ptr<OnOffApplication> app_fuente, Ptr<Queue<Packet>> cola_dispo, Ptr<QueueDisc> cola_tcl);
  void     PaqueteTx_fuente (Ptr<const Packet> paquete);
  double   GetMediaIntervaloTx (void);
  double   GetNPaquetesTx (void);
private:
  Ptr<Queue<Packet>> m_cola_dispo;
  Ptr<QueueDisc> m_cola_tcl;
  Average<double> m_media_tx;
  Time m_prev_time;
  double m_cuenta;
};