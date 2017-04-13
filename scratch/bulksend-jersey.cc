/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 NITK Surathkal
 *
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
 * Authors:   Sourabh Jain <sourabhjain560@outlook.com>
 *            Vivek Jain <jain.vivek.anand@gmail.com>
 */

#include <iostream>

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/gtk-config-store.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("BulkSendJersey");
  double stopTime = 10;

void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon)
{
/*  std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
  Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
      Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
      std::cout<<"Flow ID			: " << stats->first <<" ; "<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress<<std::endl;
//			std::cout<<"Tx Packets = " << stats->second.txPackets<<std::endl;
//			std::cout<<"Rx Packets = " << stats->second.rxPackets<<std::endl;
      std::cout<<"Duration		: "<<stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds()<<std::endl;
      std::cout<<"Last Received Packet	: "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds"<<std::endl;
      std::cout<<"Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps"<<std::endl;
      std::cout<<"---------------------------------------------------------------------------"<<std::endl;
    }
    */
        flowMon->CheckForLostPackets ();
        std::map<FlowId, FlowMonitor::FlowStats> stats = flowMon->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {     
      double Throughput = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds()- i->second.timeFirstTxPacket.GetSeconds()) /1024;
          std::cout << "Throughput: " << Throughput << " kbps\n";
    }
  Simulator::Schedule(Seconds(stopTime + 1),&ThroughputMonitor, fmhelper, flowMon);
}

int main (int argc, char *argv[])
{
  uint16_t port = 50000;

  uint32_t    nleftLeaf = 20;
  uint32_t    nrightLeaf = 20;
  uint32_t    maxWindowSize = 2000;
  bool isWindowScalingEnabled = true;
  std::string leftRate = "150Mbps";
  std::string leftDelay = "10ms";
  std::string rightRate = "150Mbps";
  std::string rightDelay = "10ms";
  std::string middleRate = "100Mbps";
  std::string middleDelay = "45ms";
  std::string tcpVariant = "ns3::TcpNewReno";
  std::string mobilityModel = "ns3::ConstantPositionMobilityModel";

  CommandLine cmd;
  cmd.Parse (argc, argv);

 /**
  * Setting Seed Value for generating random number
  */
  SeedManager::SetRun (11223344);

 /**
  * Creating a random number value generator variable
  */
  Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable> ();

 /**
  * Setting the TCP window size to be used
  * Setting the window scaling option
  * Setting TCP variant to be used
  */
  Config::SetDefault ("ns3::TcpSocketBase::MaxWindowSize", UintegerValue (maxWindowSize));
  Config::SetDefault ("ns3::TcpSocketBase::WindowScaling", BooleanValue (isWindowScalingEnabled));
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (tcpVariant));
  
  Config::SetDefault ("ns3::RedQueueDisc::Mode", StringValue ("QUEUE_MODE_PACKETS"));  
  Config::SetDefault ("ns3::RedQueueDisc::QueueLimit", UintegerValue (100));
  Config::SetDefault ("ns3::RedQueueDisc::MinTh", DoubleValue (15));
  Config::SetDefault ("ns3::RedQueueDisc::MaxTh", DoubleValue (15));
  Config::SetDefault ("ns3::RedQueueDisc::QW", DoubleValue (0.2));
  //Config::SetDefault ("ns3::RedQueueDisc::UseEcn", BooleanValue (true));
  Config::SetDefault ("ns3::RedQueueDisc::Gentle", BooleanValue (false));
  Config::SetDefault ("ns3::RedQueueDisc::Gentle", BooleanValue (false));
    
 /**
  * Creating point to point channel for bottleneck
  * Setting the date rate of the bottleneck link
  * Setting the propagation dealy of bottleneck link
  */
  PointToPointHelper left;
  left.SetDeviceAttribute  ("DataRate", StringValue (leftRate));
  left.SetChannelAttribute ("Delay", StringValue (leftDelay));

 /**
  * Creating point to point channel for edge links
  * Setting the date rate of the edge links
  * Setting the propagation dealy of edge links
  */
  PointToPointHelper right;
  right.SetDeviceAttribute    ("DataRate", StringValue (rightRate));
  right.SetChannelAttribute   ("Delay", StringValue (rightDelay));
  
 /**
  * Creating point to point channel for edge links
  * Setting the date rate of the edge links
  * Setting the propagation dealy of edge links
  */
  PointToPointHelper middle;
  middle.SetDeviceAttribute    ("DataRate", StringValue (middleRate));
  middle.SetChannelAttribute   ("Delay", StringValue (middleDelay));
  
 /**
  * Creating dumbbell topology which uses point to point channel for edge links and bottleneck using PointToPointDumbbellHelper
  */
  WirelessJerseyHelper d (nleftLeaf, nrightLeaf, left, right, middle, mobilityModel);

 /**
  * Installing Interstack in all nodes
  */
  InternetStackHelper stack;
  d.InstallStack (stack);
  
  TrafficControlHelper tchBottleneck;
  QueueDiscContainer queueDiscsLeft;
  QueueDiscContainer queueDiscsMiddle;
  QueueDiscContainer queueDiscsRight;
  tchBottleneck.SetRootQueueDisc ("ns3::RedQueueDisc");
  queueDiscsLeft = tchBottleneck.Install (d.GetLeft ()->GetDevice (0))  ;
  queueDiscsMiddle = tchBottleneck.Install (d.GetMiddle ()->GetDevice (1));
  queueDiscsRight = tchBottleneck.Install (d.GetRight ()->GetDevice (0));

  // Assign IP Addresses
  d.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                         Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"),
                         Ipv4AddressHelper ("10.3.1.0", "255.255.255.0"),
                         Ipv4AddressHelper ("10.4.1.0", "255.255.255.0"));

 Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 
  // Configure application
  for (uint16_t i = 0; i < d.LeftCount (); i++)
    {
      std::cout << i << std::endl;
      BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
      AddressValue remoteAddress (InetSocketAddress (d.GetRightIpv4Address (i), port));
      ftp.SetAttribute ("Remote", remoteAddress);
      ftp.SetAttribute ("SendSize", UintegerValue (1000));

      ApplicationContainer sourceApp = ftp.Install (d.GetLeft (i));
      sourceApp.Start (Seconds (0));
      sourceApp.Stop (Seconds (stopTime - 1));

      Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
      PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
      sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
      
      ApplicationContainer sinkApp = sinkHelper.Install (d.GetRight (i));
      sinkApp.Start (Seconds (0));
      sinkApp.Stop (Seconds (stopTime));
    }

  std::cout << "Running the simulation" << std::endl;
   
  FlowMonitorHelper fmHelper;
  Ptr<FlowMonitor> allMon = fmHelper.InstallAll();
  ThroughputMonitor(&fmHelper, allMon);  

  Simulator::Stop (Seconds (stopTime + 2));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
