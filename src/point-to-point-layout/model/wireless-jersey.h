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



#include <string>

#include "point-to-point-helper.h"
#include "ipv4-address-helper.h"
#include "ipv6-address-helper.h"
#include "internet-stack-helper.h"
#include "ipv4-interface-container.h"
#include "ipv6-interface-container.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/athstats-helper.h"
#include "ns3/config-store-module.h"

namespace ns3 {

class WirelessJerseyHelper
{
public:

  WirelessJerseyHelper (uint32_t nLeftLeaf,
                        uint32_t nRightLeaf,
                        PointToPointHelper rightHelpe,
                        PointToPointHelper leftHelper,    
                        PointToPointHelper centrallinkHelper,
			std::string mobilityModel);

  ~WirelessJerseyHelper ();

public:
  /**
   * \returns pointer to the node of the left side bottleneck
   *          router
   */
  Ptr<Node> GetLeft () const;

  /**
   * \returns pointer to the i'th left side leaf node
   * \param i node number
   */
  Ptr<Node> GetLeft (uint32_t i) const;

  /**
   * \returns pointer to the node of the right side bottleneck
   *          router
   */
  Ptr<Node> GetRight () const;

  /**
   * \returns pointer to the i'th right side leaf node
   * \param i node number
   */
  Ptr<Node> GetRight (uint32_t i) const;

  /**
   * \returns pointer to the middle router
   */
  Ptr<Node> GetMiddle () const;

  /**
   * \returns an Ipv4Address of the i'th left leaf
   * \param i node number
   */
  Ipv4Address GetLeftIpv4Address (uint32_t i ) const; // Get left leaf address

  /**
   * \returns an Ipv4Address of the i'th right leaf
   * \param i node number
   */
  Ipv4Address GetRightIpv4Address (uint32_t i) const;

  /**
   * \returns total number of left side leaf nodes
   */
  uint32_t  LeftCount () const;

  /**
   * \returns total number of right side leaf nodes
   */
  uint32_t  RightCount () const;

  /**
   * \param stack an InternetStackHelper which is used to install 
   *              on every node in the wireless-jersey
   */
  void      InstallStack (InternetStackHelper stack);

  /**
   * \param leftIp Ipv4AddressHelper to assign Ipv4 addresses to the
   *               interfaces on the left side of the wireless-jersey
   *
   * \param rightIp Ipv4AddressHelper to assign Ipv4 addresses to the
   *                interfaces on the right side of the wireless-jersey
   *
   * \param leftToMiddleRotuerIP Ipv4AddressHelper to assign Ipv4 addresses to the 
   *                 interfaces between left to middle router
   *
   * \param middleToRightIp Ipv4AddressHelper to assign Ipv4 addresses to the 
   *                 interfaces between middle router to right
   */
void AssignIpv4Addresses (Ipv4AddressHelper leftIp,
                          Ipv4AddressHelper rightIp,
                          Ipv4AddressHelper leftToMidddleIp,
                          Ipv4AddressHelper middleToRightIp);



private:
  NodeContainer          m_leftLeaf;                           //!< Left Leaf nodes
  NetDeviceContainer     m_leftLeafDevices;                    //!< Left Leaf NetDevices

  NodeContainer          m_rightLeaf;                          //!< Right Leaf nodes
  NetDeviceContainer     m_rightLeafDevices;                   //!< Right Leaf NetDevices
  
  NodeContainer          m_centralNodes;        

  NetDeviceContainer     m_leftRouterDevices;                 //!< Routers NetDevices
  NetDeviceContainer     m_rightRouterDevices;                //!< Right router NetDevices

  Ipv4InterfaceContainer m_leftLeafInterfaces;                //!< Left Leaf interfaces (IPv4)
  Ipv4InterfaceContainer m_leftRouterInterfaces;              //!< Left router interfaces (IPv4)
  Ipv4InterfaceContainer m_rightLeafInterfaces;               //!< Right Leaf interfaces (IPv4)
  Ipv4InterfaceContainer m_rightRouterInterfaces;             //!< Right router interfaces (IPv4)
  Ipv4InterfaceContainer m_middleRouterToLeftInterfaces;      //!< Router interfaces (IPv4)
  Ipv4InterfaceContainer m_middleRouterToRightInterfaces;

  WifiHelper wifi;
  MobilityHelper mobility;
  WifiMacHelper wifiMac;
  YansWifiPhyHelper wifiPhy;
  YansWifiChannelHelper wifiChannel;
  NetDeviceContainer leftToMiddleDevice;
  NetDeviceContainer middleToRightDevice;
};

} // namespace ns3
