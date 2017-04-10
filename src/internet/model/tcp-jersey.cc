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
  static TypeId tid = TypeId ("ns3::TcpJersey")
    .SetParent<TcpNewReno> ()
    .SetGroupName ("Internet")
    .AddConstructor<TcpJersey> ()
    .AddAttribute ("K", "RTT multiple constant",
                   UintegerValue (1),
                   MakeUintegerAccessor (&TcpJersey::m_K),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

TcpJersey::TcpJersey (void)
  : TcpNewReno (),
    m_currentBW (0),
    m_currentRTT (Time (0)),
    m_prevAckTime (Time (0)),
    m_tLast (Time (0)),
    m_ackedSegments (0)
{
  NS_LOG_FUNCTION (this);
}

TcpJersey::TcpJersey (const TcpJersey& sock)
  : TcpNewReno (sock),
    m_currentBW (sock.m_currentBW),
    m_currentRTT (sock.m_currentRTT),
    m_prevAckTime (Time (0)),
    m_tLast (Time (0)),
    m_ackedSegments (0)
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

  m_ackedSegments += packetsAcked;
  m_currentRTT = rtt;
  EstimateBW (tcb, rtt);
}

void
TcpJersey::EstimateBW (Ptr<TcpSocketState> tcb, const Time &rtt)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (!rtt.IsZero ());

  Time now = Simulator::Now ();

  Time Tw =  rtt * m_K;
  double delta = now.GetSeconds () - m_prevAckTime.GetSeconds ();
  m_prevAckTime = now;

  if ((now - m_tLast) >= rtt)
    {
      double temp = (Tw.GetSeconds () * m_currentBW) + (m_ackedSegments * tcb->m_segmentSize);
      m_currentBW = temp / (delta + Tw.GetSeconds ());
      m_tLast = now;
      m_ackedSegments = 0;
    }
}

uint32_t
TcpJersey::GetSsThresh (Ptr<const TcpSocketState> tcb,
                        uint32_t bytesInFlight)
{
  (void) bytesInFlight;

  uint32_t ownd = m_currentRTT.GetSeconds () * m_currentBW;

  NS_LOG_LOGIC ("CurrentBW: " << m_currentBW << " ssthresh: " << ownd);

  return std::max (2 * tcb->m_segmentSize, ownd);
}

void
TcpJersey::RateControl (Ptr<TcpSocketState> tcb, uint32_t bytesInFlight)
{
  tcb->m_ssThresh = GetSsThresh (tcb, bytesInFlight);

  if ((tcb->m_congState == TcpSocketState::CA_OPEN) || (tcb->m_congState == TcpSocketState::CA_DISORDER) || (tcb->m_congState == TcpSocketState::CA_CWR) || (tcb->m_congState == TcpSocketState::CA_RECOVERY) || (tcb->m_congState == TcpSocketState::CA_LOSS) || (tcb->m_congState == TcpSocketState::CA_LAST_STATE))
    {
      tcb->m_cWnd = tcb->m_ssThresh;
    }
}

void
TcpJersey::ExplicitRetransmit (Ptr<TcpSocketState> tcb, uint32_t bytesInFlight)
{
}

Ptr<TcpCongestionOps>
TcpJersey::Fork ()
{
  return CreateObject<TcpJersey> (*this);
}

} // namespace ns3
