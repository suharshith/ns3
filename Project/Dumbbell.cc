/*

  Project:  Improved Utility-based Congestion Control for Delay-Constrained Communication using NS-3
  Dumbbell Topology
*/
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/drop-tail-queue.h"
#include <fstream> 
#include <vector> 
#include <string>

#include <cstdlib>

using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("Dumbbell");

std::string fileNameRoot = "Dumbbell";   

void CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{

  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " <<  newCwnd << std::endl;

}



static void

TraceCwnd ()    // Trace changes to the congestion window

{

  AsciiTraceHelper ascii;

  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream (fileNameRoot + ".cwnd");

  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeBoundCallback (&CwndChange,stream));

}

int main (int argc, char *argv[])
{

int tcpSegmentSize = 1000;
unsigned int runtime = 40;  

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcpSegmentSize));

  Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (0));

  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpLedbat"));

//  Config::SetDefault ("ns3::RttEstimator::MinRTO", TimeValue (MilliSeconds (500)));


double errRate = 0.000001;

// cubic is the default congestion algorithm in Linux 2.6.26
std::string tcpCong = "cubic";

Config::SetDefault ("ns3::RateErrorModel::ErrorRate", DoubleValue(errRate));
//Config::SetDefault ("ns3::RateErrorModel::ErrorUnit", StringValue ("ERROR_UNIT_PACKET"));
  
Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (130));
Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("3.5Mbps"));

ns3::PacketMetadata::Enable();
std::string animFile = "Dumbbell-anim.xml";



    NodeContainer nodes;
    nodes.Create(10);


    NodeContainer AC = NodeContainer(nodes.Get(0), nodes.Get(2));
    NodeContainer AD = NodeContainer(nodes.Get(0), nodes.Get(3));
    NodeContainer AB = NodeContainer(nodes.Get(0), nodes.Get(1));
    NodeContainer AG = NodeContainer(nodes.Get(0), nodes.Get(6));
    NodeContainer AH = NodeContainer(nodes.Get(0), nodes.Get(7));
    NodeContainer BE = NodeContainer(nodes.Get(1), nodes.Get(4));
    NodeContainer BF = NodeContainer(nodes.Get(1), nodes.Get(5));
    NodeContainer BI = NodeContainer(nodes.Get(1), nodes.Get(8));
    NodeContainer BJ = NodeContainer(nodes.Get(1), nodes.Get(9));



std::string AC_BW ("0.5Mbps");
std::string AC_Delay ("100ms");
std::string AB_Delay ("25ms");
CommandLine cmd;
cmd.AddValue ("AC_BW", "A-C Link Bandwidth", AC_BW);
cmd.AddValue ("AC_Delay", "A-C Link Delay", AC_Delay);
cmd.AddValue ("AB_Delay", "A-B Link Delay", AB_Delay);

//cmd.AddValue("TCP_CONGESTION", "Linux 2.6.26 Tcp Congestion control algorithm to use", tcpCong);
//cmd.AddValue("error-rate", "Error rate to apply to link", errRate);

cmd.Parse (argc, argv);

PointToPointHelper p2p;
p2p.SetDeviceAttribute ("DataRate", StringValue(AC_BW));
p2p.SetChannelAttribute ("Delay", StringValue(AC_Delay));

NetDeviceContainer dAC = p2p.Install (AC);
NetDeviceContainer dAD = p2p.Install (AD);
NetDeviceContainer dAG = p2p.Install (AG);
NetDeviceContainer dAH = p2p.Install (AH);
NetDeviceContainer dBI = p2p.Install (BI);
NetDeviceContainer dBJ = p2p.Install (BJ);
NetDeviceContainer dBE = p2p.Install (BE);
NetDeviceContainer dBF = p2p.Install (BF);

//p2p.SetDeviceAttribute ("DataRate", StringValue("10Mbps"));
p2p.SetChannelAttribute ("Delay", StringValue(AB_Delay));
p2p.SetQueue ("ns3::DropTailQueue","MaxPackets", UintegerValue (140));

NetDeviceContainer dAB = p2p.Install (AB);


InternetStackHelper internet;
internet.Install (nodes);
Ipv4AddressHelper ipv4;
ipv4.SetBase ("10.1.1.0", "255.255.255.0");
Ipv4InterfaceContainer iAC = ipv4.Assign (dAC);
ipv4.SetBase ("10.1.2.0", "255.255.255.0");
Ipv4InterfaceContainer iAD = ipv4.Assign (dAD);
ipv4.SetBase ("10.1.3.0", "255.255.255.0");
Ipv4InterfaceContainer iAB = ipv4.Assign (dAB);
ipv4.SetBase ("10.1.4.0", "255.255.255.0");
Ipv4InterfaceContainer iBE = ipv4.Assign (dBE);
ipv4.SetBase ("10.1.5.0", "255.255.255.0");
Ipv4InterfaceContainer iBF = ipv4.Assign (dBF);
ipv4.SetBase ("10.1.6.0", "255.255.255.0");
Ipv4InterfaceContainer iAG = ipv4.Assign (dAG);
ipv4.SetBase ("10.1.7.0", "255.255.255.0");
Ipv4InterfaceContainer iAH = ipv4.Assign (dAH);
ipv4.SetBase ("10.1.8.0", "255.255.255.0");
Ipv4InterfaceContainer iBI = ipv4.Assign (dBI);
ipv4.SetBase ("10.1.9.0", "255.255.255.0");
Ipv4InterfaceContainer iBJ = ipv4.Assign (dBJ);

Ipv4GlobalRoutingHelper::PopulateRoutingTables ();



// AB ERROR

Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
em->SetAttribute ("ErrorRate", DoubleValue(errRate));
dAB.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));






uint16_t TCPport = 8080;
uint16_t UDPport = 9;
NodeContainer E = NodeContainer (nodes.Get (4));
NodeContainer F = NodeContainer (nodes.Get (5));
NodeContainer C = NodeContainer (nodes.Get (2));
NodeContainer D = NodeContainer (nodes.Get (3));
NodeContainer G = NodeContainer (nodes.Get (6));
NodeContainer H = NodeContainer (nodes.Get (7));
NodeContainer I = NodeContainer (nodes.Get (8));
NodeContainer J = NodeContainer (nodes.Get (9));

ApplicationContainer App1;
ApplicationContainer App2;
ApplicationContainer App3;
ApplicationContainer App4;

PacketSinkHelper TCPsink1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), TCPport));
PacketSinkHelper TCPsink2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), TCPport));
PacketSinkHelper TCPsink3 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), TCPport));
PacketSinkHelper UDPsink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), UDPport));



//UDP Connection - between H and J
App1 = UDPsink.Install (nodes.Get (9));//Node J
App1.Start (Seconds (0.0));
App1.Stop (Seconds (20.0));

Address J_Address_UDP(InetSocketAddress(iBJ.GetAddress (1), UDPport));

OnOffHelper UDPsource ("ns3::UdpSocketFactory", J_Address_UDP); 
UDPsource.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
UDPsource.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
App1 = UDPsource.Install(nodes.Get(7));//Node H
App1.Start (Seconds (0.0));
App1.Stop (Seconds (20.0));



//TCP connection no 2 - between D and F
App2 = TCPsink1.Install (F); // Install sink app on node F 
App2.Start (Seconds (2.0));
App2.Stop (Seconds (20.0));

Address F_Address(InetSocketAddress(iBF.GetAddress (1), TCPport));

OnOffHelper TCPsource1 ("ns3::TcpSocketFactory", F_Address); 
TCPsource1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
TCPsource1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
App2 = TCPsource1.Install(nodes.Get(3));//Node D
App2.Start (Seconds (2.0));
App2.Stop (Seconds (20.0));


//TCP connection no 3 - between G and I
App3 = TCPsink2.Install (I); // Install sink app on node I
App3.Start (Seconds (0.0));
App3.Stop (Seconds (20.0));

Address I_Address_TCP(InetSocketAddress(iBI.GetAddress (1), TCPport));

OnOffHelper TCPsource2 ("ns3::TcpSocketFactory", I_Address_TCP); 
TCPsource2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
TCPsource2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
App3 = TCPsource2.Install(nodes.Get(6));//Node G
App3.Start (Seconds (4.0));
App3.Stop (Seconds (20.0));

//TCP connection no 1 - between C and E
App4 = TCPsink3.Install (E); // Install sink app on node E
App4.Start (Seconds (0.0));
App4.Stop (Seconds (20.0));

Address E_Address_TCP(InetSocketAddress(iBE.GetAddress (1), TCPport));

OnOffHelper TCPsource3 ("ns3::TcpSocketFactory", E_Address_TCP); 
TCPsource3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
TCPsource3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
App4 = TCPsource3.Install(nodes.Get(2));//Node C
App4.Start (Seconds (1.0));
App4.Stop (Seconds (20.0));



AnimationInterface anim (animFile);
Ptr<Node> n = nodes.Get (0);
anim.SetConstantPosition (n,20.0,70.0);
n = nodes.Get (1);
anim.SetConstantPosition (n,40,70);
n = nodes.Get (2);
anim.SetConstantPosition (n,0,40);
n = nodes.Get (3);
anim.SetConstantPosition (n,0,60);
n = nodes.Get (4);
anim.SetConstantPosition (n,60,40);
n = nodes.Get (5);
anim.SetConstantPosition (n,60,60);
n = nodes.Get (6);
anim.SetConstantPosition (n,0,80);
n = nodes.Get (7);
anim.SetConstantPosition (n,0,100);
n = nodes.Get (8);
anim.SetConstantPosition (n,60,80);
n = nodes.Get (9);
anim.SetConstantPosition (n,60,100);

AsciiTraceHelper ascii;
p2p.EnableAsciiAll (ascii.CreateFileStream (fileNameRoot+".tr"));
p2p.EnablePcapAll(fileNameRoot);

Simulator::Schedule(Seconds(0.01),&TraceCwnd);


Simulator::Stop (Seconds (runtime+60));

std::cout << "Running the simulation" << std::endl;

Simulator::Run ();
std::cout << "Destroying the simulation" << std::endl;

//Simulator::Destroy ();
return 0;

}








