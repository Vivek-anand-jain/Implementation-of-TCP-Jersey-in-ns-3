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

#ifndef TCP_JERSEY_H
#define TCP_JERSEY_H

#include "tcp-congestion-ops.h"
#include "ns3/sequence-number.h"
#include "ns3/traced-value.h"

namespace ns3 {

class Packet;
class TcpHeader;
class Time;

/**
 * \ingroup congestionOps
 *
 * \brief An implementation of TCP Westwood and Westwood+.
 *
 * Westwood and Westwood+ employ the AIAD (Additive Increase/Adaptive Decrease) 
 * congestion control paradigm. When a congestion episode happens, 
 * instead of halving the cwnd, these protocols try to estimate the network's
 * bandwidth and use the estimated value to adjust the cwnd. 
 * While Westwood performs the bandwidth sampling every ACK reception, 
 * Westwood+ samples the bandwidth every RTT.
 *
 * The two main methods in the implementation are the CountAck (const TCPHeader&)
 * and the EstimateBW (int, const, Time). The CountAck method calculates
 * the number of acknowledged segments on the receipt of an ACK.
 * The EstimateBW estimates the bandwidth based on the value returned by CountAck
 * and the sampling interval (last ACK inter-arrival time for Westwood and last RTT for Westwood+).
 */
class TcpJersey : public TcpNewReno
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  TcpJersey (void);
  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  TcpJersey (const TcpJersey& sock);
  virtual ~TcpJersey (void);

  virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb,
                                uint32_t bytesInFlight);

  virtual void RateControl (Ptr<TcpSocketState> tcb, uint32_t bytesInFlight);
  virtual void ExplicitRetransmit (Ptr<TcpSocketState> tcb, uint32_t bytesInFlight);

  virtual void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t packetsAcked,
                          const Time& rtt);

  virtual Ptr<TcpCongestionOps> Fork ();

private:

  /**
   * Estimate the network's bandwidth
   *
   * \param [in] rtt the RTT estimation.
   * \param [in] tcb the socket state.
   */
  void EstimateBW (Ptr<TcpSocketState> tcb, const Time& rtt);

protected:
  double                 m_currentBW;              //!< Current value of the estimated BW
  Time                   m_currentRTT;             //!< Current Value of the RTT
  Time                   m_prevAckTime;            //!< 
  Time                   m_tLast;                  //!< 
  int                    m_ackedSegments;          //!< The number of segments ACKed between RTTs
  uint32_t               m_K;
};

} // namespace ns3

#endif /* TCP_JERSEY_H */
