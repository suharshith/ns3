/*

  Project:  Improved Utility-based Congestion Control for Delay-Constrained Communication using NS-3
  Parking Lot Topology
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


NS_LOG_COMPONENT_DEFINE ("parking");

std::string fileNameRoot = "Parking-lot";   

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


double errRate = 0.000001;

// cubic is the default congestion algorithm in Linux 2.6.26
//std::string tcpCong = "cubic";

int tcpSegmentSize = 1000;
unsigned int runtime = 20;  

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcpSegmentSize));

  Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (0));

  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpLedbat"));

//  Config::SetDefault ("ns3::RttEstimator::MinRTO", TimeValue (MilliSeconds (500)));

//Config::SetDefault ("ns3::RateErrorModel::ErrorRate", Value(errRate));
Config::SetDefault ("ns3::RateErrorModel::ErrorRate", DoubleValue(errRate));
//Config::SetDefault ("ns3::RateErrorModel::ErrorUnit", StringValue ("ERROR_UNIT_PACKET"));
Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (100));
//Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("1Mbps"));

//char protocol = "ns3::TcpL4Protocol::SocketType";


// Output config store to txt format
//Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("output-attributes.txt"));
//Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("RawText"));
//Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
//ConfigStore outputConfig2;
//outputConfig2.ConfigureDefaults ();
//outputConfig2.ConfigureAttributes ();


//std::cout<<Config::GetRootNamespaceObject(0)<<std::endl;
//std::cout<<"protocol is" << protocol <<std::endl;

// std::cout << "Protocol set is " << Config <<std::endl;

ns3::PacketMetadata::Enable();

NodeContainer nodes;
nodes.Create(9);


NodeContainer AB = NodeContainer(nodes.Get(0), nodes.Get(1));
NodeContainer BC = NodeContainer(nodes.Get(1), nodes.Get(2));
NodeContainer CD = NodeContainer(nodes.Get(2), nodes.Get(3));
NodeContainer DE = NodeContainer(nodes.Get(3), nodes.Get(4));
NodeContainer BF = NodeContainer(nodes.Get(1), nodes.Get(5));
NodeContainer BG = NodeContainer(nodes.Get(1), nodes.Get(6));
NodeContainer CH = NodeContainer(nodes.Get(2), nodes.Get(7));
NodeContainer DI = NodeContainer(nodes.Get(3), nodes.Get(8));


std::string TCP_BW ("2.5Mbps");
std::string Delay ("25ms");
std::string UDP_BW ("500Kbps");

CommandLine cmd;
cmd.AddValue ("TCP_BW", "TCP Link Bandwidth", TCP_BW);
cmd.AddValue ("Delay", "Link Delay", Delay);
cmd.AddValue ("UDP_BW", "UDP Link Bandwidth", UDP_BW);

//cmd.AddValue("TCP_CONGESTION", "Linux 2.6.26 Tcp Congestion control algorithm to use", tcpCong);
//cmd.AddValue("error-rate", "Error rate to apply to link", errRate);

cmd.Parse (argc, argv);

PointToPointHelper p2p;
p2p.SetDeviceAttribute ("DataRate", StringValue(TCP_BW));
p2p.SetChannelAttribute ("Delay", StringValue(Delay));

NetDeviceContainer dAB = p2p.Install (AB);
NetDeviceContainer dBC = p2p.Install (BC);
NetDeviceContainer dCD = p2p.Install (CD);
NetDeviceContainer dDE = p2p.Install (DE);
NetDeviceContainer dBF = p2p.Install (BF);
NetDeviceContainer dBG = p2p.Install (BG);
NetDeviceContainer dCH = p2p.Install (CH);
NetDeviceContainer dDI = p2p.Install (DI);

//p2p.SetDeviceAttribute ("DataRate", StringValue("10Mbps"));
p2p.SetChannelAttribute ("Delay", StringValue(Delay));
p2p.SetQueue ("ns3::DropTailQueue","MaxPackets", UintegerValue (100));

InternetStackHelper internet;
internet.Install (nodes);
Ipv4AddressHelper ipv4;
ipv4.SetBase ("10.1.1.0", "255.255.255.0");
Ipv4InterfaceContainer iAB = ipv4.Assign (dAB);
ipv4.SetBase ("10.1.2.0", "255.255.255.0");
Ipv4InterfaceContainer iBC = ipv4.Assign (dBC);
ipv4.SetBase ("10.1.3.0", "255.255.255.0");
Ipv4InterfaceContainer iCD = ipv4.Assign (dCD);
ipv4.SetBase ("10.1.4.0", "255.255.255.0");
Ipv4InterfaceContainer iDE = ipv4.Assign (dDE);
ipv4.SetBase ("10.1.5.0", "255.255.255.0");
Ipv4InterfaceContainer iBF = ipv4.Assign (dBF);
ipv4.SetBase ("10.1.6.0", "255.255.255.0");
Ipv4InterfaceContainer iBG = ipv4.Assign (dBG);
ipv4.SetBase ("10.1.7.0", "255.255.255.0");
Ipv4InterfaceContainer iCH = ipv4.Assign (dCH);
ipv4.SetBase ("10.1.8.0", "255.255.255.0");
Ipv4InterfaceContainer iDI = ipv4.Assign (dDI);

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
NodeContainer B = NodeContainer (nodes.Get (1));

ApplicationContainer App1;
ApplicationContainer App2;
ApplicationContainer App3;
ApplicationContainer App4;

PacketSinkHelper TCPsink1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), TCPport));
PacketSinkHelper TCPsink2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), TCPport));
PacketSinkHelper TCPsink3 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), TCPport));
PacketSinkHelper UDPsink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), UDPport));



//UDP Connection - between A and E
App1 = UDPsink.Install (nodes.Get (4));//Node E
App1.Start (Seconds (1.0));
App1.Stop (Seconds (20.0));

Address E_Address_UDP(InetSocketAddress(iDE.GetAddress (1), UDPport));

OnOffHelper UDPsource ("ns3::UdpSocketFactory", E_Address_UDP); 
UDPsource.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
UDPsource.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
App1 = UDPsource.Install(nodes.Get(0));//Node A
App1.Start (Seconds (0.0));
App1.Stop (Seconds (20.0));



//TCP connection no 1 - between E and F
App2 = TCPsink1.Install (E); // Install sink app on node E 
App2.Start (Seconds (0.0));
App2.Stop (Seconds (20.0));

Address E_Address(InetSocketAddress(iDE.GetAddress (1), TCPport));

OnOffHelper TCPsource1 ("ns3::TcpSocketFactory", E_Address); 
TCPsource1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
TCPsource1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
App2 = TCPsource1.Install(nodes.Get(5));//Node F
App2.Start (Seconds (1.0));
App2.Stop (Seconds (20.0));


//TCP connection no 2 - between G and H
App3 = TCPsink2.Install (H); // Install sink app on node H
App3.Start (Seconds (0.0));
App3.Stop (Seconds (20.0));

Address H_Address_TCP(InetSocketAddress(iCH.GetAddress (1), TCPport));

OnOffHelper TCPsource2 ("ns3::TcpSocketFactory", H_Address_TCP); 
TCPsource2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
TCPsource2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
App3 = TCPsource2.Install(nodes.Get(6));//Node G
App3.Start (Seconds (1.0));
App3.Stop (Seconds (20.0));

//TCP connection no 3 - between H and I
App4 = TCPsink3.Install (I); // Install sink app on node I
App4.Start (Seconds (0.0));
App4.Stop (Seconds (20.0));

Address I_Address_TCP(InetSocketAddress(iDI.GetAddress (1), TCPport));

OnOffHelper TCPsource3 ("ns3::TcpSocketFactory", I_Address_TCP); 
TCPsource3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
TCPsource3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
App4 = TCPsource3.Install(nodes.Get(7));//Node H
App4.Start (Seconds (1.0));
App4.Stop (Seconds (20.0));



AnimationInterface anim ("Parking-lot-anim.xml");

anim.SetConstantPosition(nodes.Get(0),0.0,75.0);
anim.SetConstantPosition(nodes.Get(1),50.0,75.0);
anim.SetConstantPosition(nodes.Get(2),100.0,75.0);
anim.SetConstantPosition(nodes.Get(3),150.0,75.0);
anim.SetConstantPosition(nodes.Get(4),200.0,75.0);
anim.SetConstantPosition(nodes.Get(5),50.0,25.0);
anim.SetConstantPosition(nodes.Get(6),50.0,125.0);
anim.SetConstantPosition(nodes.Get(7),100.0,125.0);
anim.SetConstantPosition(nodes.Get(8),150.0,125.0);


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







