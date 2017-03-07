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
 * Authors: Vivek Jain <jain.vivek.anand@gmail.com>
 *          Sourabh Jain <sourabhjain560@outlook.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "tcp-jersey.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "rtt-estimator.h"
#include "tcp-socket-base.h"

NS_LOG_COMPONENT_DEFINE ("TcpJersey");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TcpJersey);

TypeId
TcpJersey::GetTypeId (void)
{
  static TypeId tid = TypeId("ns3::TcpJersey")
    .SetParent<TcpNewReno>()
    .SetGroupName ("Internet")
    .AddConstructor<TcpJersey>()
  ;
  return tid;
}

TcpJersey::TcpJersey (void) :
  TcpNewReno (),
  m_currentBW (0),
  m_lastBW (0),
  m_currentRTT (Time (0)),
  m_lastRTT (Time (0)),
  m_lastAckTime (Time (0))
{
  NS_LOG_FUNCTION (this);
}

TcpJersey::TcpJersey (const TcpJersey& sock) :
  TcpNewReno (sock),
  m_currentBW (sock.m_currentBW),
  m_lastBW (sock.m_lastBW),
  m_currentRTT (sock.m_currentRTT),
  m_lastRTT (Time (0)),
  m_lastAckTime (Time (0))

{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Invoked the copy constructor");
}

TcpJersey::~TcpJersey (void)
{
}

void
TcpJersey::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t packetsAcked,
                        const Time& rtt)
{
  NS_LOG_FUNCTION (this << tcb << packetsAcked << rtt);

  if (rtt.IsZero ())
    {
      NS_LOG_WARN ("RTT measured is zero!");
      return;
    }
  m_ackedSegments = packetsAcked;
  m_currentAckTime = Simulator::Now ();
}

void
TcpJersey::EstimateBW (const Time &rtt, Ptr<TcpSocketState> tcb)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (!rtt.IsZero ());

  m_currentRTT = rtt;

  m_currentBW = (m_currentRTT.GetSeconds () * m_lastBW + (m_ackedSegments * tcb->m_segmentSize)) / (m_currentRTT.GetSeconds () + (m_currentAckTime.GetSeconds () - m_lastAckTime.GetSeconds ()));
  NS_LOG_LOGIC ("Estimated BW: " << m_currentBW);

  m_lastAckTime = m_currentAckTime;
  m_lastRTT = m_currentRTT;
  m_lastBW = m_currentBW;
}

uint32_t
TcpJersey::GetSsThresh (Ptr<const TcpSocketState> tcb,
                          uint32_t bytesInFlight)
{
  (void) bytesInFlight;

  uint32_t ownd = m_currentRTT.GetSeconds () * m_currentBW / tcb->m_segmentSize;

  if (ownd < 2)
    {
      ownd = 2;
    }

  NS_LOG_LOGIC ("CurrentBW: " << m_currentBW << " ssthresh: " << ownd);

  return ownd;
}

Ptr<TcpCongestionOps>
TcpJersey::Fork ()
{
  return CreateObject<TcpJersey> (*this);
}

} // namespace ns3
