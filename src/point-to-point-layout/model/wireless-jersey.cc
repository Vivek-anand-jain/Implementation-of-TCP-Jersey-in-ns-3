/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Sourabh Jain <jainsourabh679@gmail.com>
 *         Vivek Jain	<jain.vivek.anand@gmail.com>
 */


#include <cmath>
#include <iostream>
#include <sstream>

// ns3 includes
#include "ns3/log.h"

#include "ns3/constant-position-mobility-model.h"

#include "ns3/node-list.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/vector.h"
#include "ns3/ipv6-address-generator.h"

#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wireless-jersey.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("WirelessJerseyHelper");

WirelessJerseyHelper::WirelessJerseyHelper (uint32_t nLeftLeaf,
                                            uint32_t nRightLeaf,
                                            PointToPointHelper leftHelper,
                                            PointToPointHelper rightHelper,
					    PointToPointHelper bottleneckHelper,
                                            std::string mobilityModel)
{
  
  m_centralNodes.Create(3);
  NodeContainer nc1;
  nc1.Add (m_centralNodes.Get (0));
  nc1.Add (m_centralNodes.Get (1));
  
  NodeContainer nc2;
  nc2.Add (m_centralNodes.Get (1));
  nc2.Add (m_centralNodes.Get (2));
  
  // Create the leaf nodes
  m_leftLeaf.Create (nLeftLeaf);
  
  // Create the right nodes
  m_rightLeaf.Create (nRightLeaf);

  // Add the link connecting routers
  leftToMiddleDevice = bottleneckHelper.Install (nc1);
  
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("DsssRate2Mbps"),
                                "ControlMode", StringValue ("DsssRate1Mbps"));

  WifiMacHelper mac;
  mac.SetType ("ns3::AdhocWifiMac");
  
  MobilityHelper mobility;
  mobility.SetMobilityModel (mobilityModel);
  mobility.Install (nc2);

  NetDeviceContainer devices;
  devices = wifi.Install (phy, mac, nc2);
  
  middleToRightDevice.Add (devices);


  
  // Add the left side links
  for(uint32_t i = 0; i < nLeftLeaf;  ++i)
  {
        NetDeviceContainer c = leftHelper.Install (m_centralNodes.Get (0),
                                          m_leftLeaf.Get (i));
        m_leftRouterDevices.Add (c.Get (0));
        m_leftLeafDevices.Add (c.Get (1));
  }
  
  
  // Add the right side links
  for (uint32_t i = 0; i < nRightLeaf; ++i)
    {
      NetDeviceContainer c = rightHelper.Install (m_centralNodes.Get (2),
                                                  m_rightLeaf.Get (i));
      m_rightRouterDevices.Add (c.Get (0));
      m_rightLeafDevices.Add (c.Get (1));
    }
}

WirelessJerseyHelper::~WirelessJerseyHelper ()
{

}

Ptr<Node> WirelessJerseyHelper::GetLeft () const
{ // Get the left side bottleneck router
  return m_centralNodes.Get (0);
}

Ptr<Node> WirelessJerseyHelper::GetLeft (uint32_t i) const
{ // Get the i'th left side leaf
  return m_leftLeaf.Get (i);
}

Ptr<Node> WirelessJerseyHelper::GetRight () const
{ // Get the right side bottleneck router
  return m_centralNodes.Get (2);
}

Ptr<Node> WirelessJerseyHelper::GetRight (uint32_t i) const
{ // Get the i'th right side leaf
  return m_rightLeaf.Get (i);
}

Ptr<Node> WirelessJerseyHelper::GetMiddle () const
{ 
  return m_centralNodes.Get (1);
}

Ipv4Address WirelessJerseyHelper::GetLeftIpv4Address (uint32_t i) const
{
  return m_leftLeafInterfaces.GetAddress (i);
}

Ipv4Address WirelessJerseyHelper::GetRightIpv4Address (uint32_t i) const
{
  return m_rightLeafInterfaces.GetAddress (i);
}

uint32_t  WirelessJerseyHelper::LeftCount () const
{ // Number of left side nodes
  return m_leftLeaf.GetN ();
}

uint32_t  WirelessJerseyHelper::RightCount () const
{ // Number of right side nodes
  return m_rightLeaf.GetN ();
}

void WirelessJerseyHelper::InstallStack (InternetStackHelper stack)
{
  stack.Install (m_centralNodes);
  stack.Install (m_leftLeaf);
  stack.Install (m_rightLeaf);
}

void WirelessJerseyHelper::AssignIpv4Addresses (Ipv4AddressHelper leftIp,
                                                Ipv4AddressHelper rightIp,
                                                Ipv4AddressHelper leftToMidddleIp,
                                                Ipv4AddressHelper middleToRightIp)
{

   // Assign to left side 
  for (uint32_t i = 0; i < LeftCount (); ++i)
    {
     NetDeviceContainer ndc;
     ndc.Add (m_leftLeafDevices.Get (i));
     ndc.Add (m_leftRouterDevices.Get (i));
     Ipv4InterfaceContainer ifc = leftIp.Assign (ndc);
     m_leftLeafInterfaces.Add (ifc.Get (0));
     m_leftRouterInterfaces.Add (ifc.Get (1));
     leftIp.NewNetwork ();
    }

   // Assign to right side 
  for (uint32_t i = 0; i < RightCount (); ++i)
   {
     NetDeviceContainer ndc;
     ndc.Add (m_rightLeafDevices.Get (i));
     ndc.Add (m_rightRouterDevices.Get (i));
     Ipv4InterfaceContainer ifc = rightIp.Assign (ndc);
     m_rightLeafInterfaces.Add (ifc.Get (0));
     m_rightRouterInterfaces.Add (ifc.Get (1));
     rightIp.NewNetwork ();
   }
  
  m_middleRouterToLeftInterfaces = leftToMidddleIp.Assign (leftToMiddleDevice);
  m_middleRouterToRightInterfaces = middleToRightIp.Assign (middleToRightDevice);
}

} // namespace ns3
