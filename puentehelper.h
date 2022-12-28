#include "ns3/ptr.h"
#include "ns3/udp-server.h"
#include "ns3/nstime.h"
 #include "ns3/simulator.h"
 #include "ns3/average.h"
 #include "ns3/point-to-point-net-device.h"

 #include "ns3/node-container.h"
 #include "ns3/net-device-container.h"
 #include "ns3/bridge-helper.h"
 #include "ns3/csma-helper.h"

 #include "ns3/data-rate.h"

 #include "ns3/names.h"

 using namespace ns3;

class PuenteHelper
{
public:
              
    Ptr<Node> Puentehelper (NodeContainer nodosLan,NetDeviceContainer & d_nodosLan,DataRate tasa_envio);
  
private:
};