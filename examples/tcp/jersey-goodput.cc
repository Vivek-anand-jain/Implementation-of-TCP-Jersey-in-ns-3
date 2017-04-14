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
 * Authors: Vivek Jain <jain.vivek.anand@gmail.com>
 *          Sourabh Jain <sourabhjain560@outlook.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-helper.h"

using namespace ns3;

int
main (int argc, char *argv[])
{
  uint16_t port = 50000;
  double stopTime = 10;
  std::string leftRate = "10Mbps";
  std::string leftDelay = "45ms";
  uint32_t    maxWindowSize = 2000;
  bool isWindowScalingEnabled = true;
  std::string tcp = "ns3::TcpJersey";
  std::string mobilityModel = "ns3::ConstantPositionMobilityModel";
  
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Config::SetDefault ("ns3::TcpSocketBase::MaxWindowSize", UintegerValue (maxWindowSize));
  Config::SetDefault ("ns3::TcpSocketBase::WindowScaling", BooleanValue (isWindowScalingEnabled));
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcp));

  NodeContainer wifiNodes;
  wifiNodes.Create (2);
  
  NodeContainer wireNodes;
  wireNodes.Create (1);
  
  PointToPointHelper leftHelper;
  leftHelper.SetDeviceAttribute    ("DataRate", StringValue (leftRate));
  leftHelper.SetChannelAttribute   ("Delay", StringValue (leftDelay));
  
  NetDeviceContainer leftNetDevices = leftHelper.Install (wireNodes.Get (0), wifiNodes.Get(0));
  
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

  NetDeviceContainer rightNetDevices;
  rightNetDevices = wifi.Install (phy, mac, wifiNodes);

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiNodes);
  
  InternetStackHelper stack;
  stack.Install (wireNodes);
  stack.Install (wifiNodes);


  Ipv4AddressHelper leftAddress;
  leftAddress.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer leftInterfaces = leftAddress.Assign (leftNetDevices);
  
  Ipv4AddressHelper rightAddress;
  rightAddress.SetBase ("10.2.1.0", "255.255.255.0");
  
  Ipv4InterfaceContainer rightInterfaces = rightAddress.Assign (rightNetDevices);
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
  AddressValue remoteAddress (InetSocketAddress (rightInterfaces.GetAddress (1), port));
  ftp.SetAttribute ("Remote", remoteAddress);
  ftp.SetAttribute ("SendSize", UintegerValue (1000));

  ApplicationContainer sourceApp = ftp.Install (wireNodes.Get (0));
  sourceApp.Start (Seconds (0));
  sourceApp.Stop (Seconds (stopTime - 1));

  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  
  ApplicationContainer sinkApp = sinkHelper.Install (wifiNodes.Get (1));
  sinkApp.Start (Seconds (0));
  sinkApp.Stop (Seconds (stopTime));
  
  
  phy.EnablePcapAll ("good-put");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
