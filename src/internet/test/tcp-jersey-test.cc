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
#include "ns3/test.h"
#include "ns3/log.h"
#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-socket-base.h"
#include "ns3/tcp-jersey.h"
#include "ns3/string.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpJerseyTestSuite");

/**
 * \ingroup internet-test
 * \ingroup tests
 *
 * \brief Jersey should be same as NewReno during slow start, and when timestamps are disabled
 */
class TcpJerseyToNewReno : public TestCase
{
public:
  /**
   * \brief Constructor
   *
   * \param cWnd congestion window
   * \param segmentSize segment size
   * \param ssThresh slow start threshold
   * \param segmentsAcked segments acked
   * \param highTxMark high tx mark
   * \param lastAckedSeq last acked seq
   * \param rtt RTT
   * \param name Name of the test
   */
  TcpJerseyToNewReno (uint32_t cWnd, uint32_t segmentSize, uint32_t ssThresh,
                      uint32_t segmentsAcked, SequenceNumber32 highTxMark,
                      SequenceNumber32 lastAckedSeq, Time rtt, const std::string &name);

private:
  virtual void DoRun (void);
  /** \brief Execute the test
   */
  void ExecuteTest (void);

  uint32_t m_cWnd; //!< cWnd
  uint32_t m_segmentSize; //!< segment size
  uint32_t m_segmentsAcked; //!< segments acked
  uint32_t m_ssThresh; //!< ss thresh
  Time m_rtt; //!< rtt
  SequenceNumber32 m_highTxMark; //!< high tx mark
  SequenceNumber32 m_lastAckedSeq; //!< last acked seq
  Ptr<TcpSocketState> m_state; //!< state
};

TcpJerseyToNewReno::TcpJerseyToNewReno (uint32_t cWnd, uint32_t segmentSize, uint32_t ssThresh,
                                        uint32_t segmentsAcked, SequenceNumber32 highTxMark,
                                        SequenceNumber32 lastAckedSeq, Time rtt, const std::string &name)
  : TestCase (name),
    m_cWnd (cWnd),
    m_segmentSize (segmentSize),
    m_segmentsAcked (segmentsAcked),
    m_ssThresh (ssThresh),
    m_rtt (rtt),
    m_highTxMark (highTxMark),
    m_lastAckedSeq (lastAckedSeq)
{
}

void
TcpJerseyToNewReno::DoRun ()
{
  Simulator::Schedule (Seconds (0.0), &TcpJerseyToNewReno::ExecuteTest, this);
  Simulator::Run ();
  Simulator::Destroy ();
}

void
TcpJerseyToNewReno::ExecuteTest ()
{
  m_state = CreateObject <TcpSocketState> ();
  m_state->m_cWnd = m_cWnd;
  m_state->m_ssThresh = m_ssThresh;
  m_state->m_segmentSize = m_segmentSize;
  m_state->m_highTxMark = m_highTxMark;
  m_state->m_lastAckedSeq = m_lastAckedSeq;

  Ptr<TcpSocketState> state = CreateObject <TcpSocketState> ();
  state->m_cWnd = m_cWnd;
  state->m_ssThresh = m_ssThresh;
  state->m_segmentSize = m_segmentSize;
  state->m_highTxMark = m_highTxMark;
  state->m_lastAckedSeq = m_lastAckedSeq;

  Ptr<TcpJersey> cong = CreateObject <TcpJersey> ();
  cong->IncreaseWindow (m_state, m_segmentsAcked);

  Ptr<TcpNewReno> NewRenoCong = CreateObject <TcpNewReno> ();
  NewRenoCong->IncreaseWindow (state, m_segmentsAcked);

  NS_TEST_ASSERT_MSG_EQ (m_state->m_cWnd.Get (), state->m_cWnd.Get (),
                         "cWnd has not updated correctly");
}

/**
 * \ingroup internet-test
 * \ingroup tests
 *
 * \brief TCP Jersey TestSuite
 */
class TcpJerseyTestSuite : public TestSuite
{
public:
  TcpJerseyTestSuite () : TestSuite ("tcp-jersey-test", UNIT)
  {
    AddTestCase (new TcpJerseyToNewReno (2 * 1446, 1446, 4 * 1446, 2, SequenceNumber32 (4753), SequenceNumber32 (3216), MilliSeconds (100), "Jersey falls to New Reno for slowstart"), TestCase::QUICK);
    AddTestCase (new TcpJerseyToNewReno (4 * 1446, 1446, 2 * 1446, 2, SequenceNumber32 (4753), SequenceNumber32 (3216), MilliSeconds (100), "Jersey falls to New Reno if timestamps are not found"), TestCase::QUICK);
  }
};

static TcpJerseyTestSuite g_tcpjerseyTest; //!< static var for test initialization
