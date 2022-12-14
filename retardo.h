#include "ns3/ptr.h"
#include "ns3/udp-server.h"

#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/average.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/application-container.h"

#include "timetag.h"

using namespace ns3;

class Retardo
{
public:
           Retardo      (ApplicationContainer c_app_fuentes, int num_fuentes, Ptr<UdpServer> receptor);
  void     PaqueteTransmitido (Ptr<const Packet> paquete);
  void     PaqueteRecibido (Ptr<const Packet> paquete);
  double   TotalPaquetesTx(void);
  Time RetardoMedio   ();

private:
  Time m_tiempo_tx;
  Time m_tiempo_rx;
  double m_cuenta;
  Average<double> media_retardo;
};