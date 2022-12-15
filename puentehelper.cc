#include "puentehelper.h"


 
 Ptr<Node> 
 PuenteHelper::Puentehelper (NodeContainer nodosLan,NetDeviceContainer & d_nodosLan,DataRateValue tasa_envio)
 {
   NetDeviceContainer d_puertosBridge;
  CsmaHelper         h_csma;
  BridgeHelper       h_bridge;
  Ptr<Node>          puente = CreateObject<Node> ();
  
  sh_csma.SetChannelAttribute("DataRate", tasa_envio);
  for (NodeContainer::Iterator indice = nodosLan.Begin ();
       indice != nodosLan.End ();
       indice++)
    {
      NetDeviceContainer enlace = h_csma.Install (NodeContainer (*indice,
                                                                 puente));
          
      d_nodosLan.Add (enlace.Get (0));
      d_puertosBridge.Add (enlace.Get (1));
    }
  h_bridge.Install (puente, d_puertosBridge);
  return puente;
}