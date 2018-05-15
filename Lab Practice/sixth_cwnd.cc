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
 */

#include <fstream>
#include <stdlib.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"

#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/csma-module.h"

// #include "ns3/tcp-nack-addition.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SixthScriptExample");

uint32_t counter = 0;

static void CwndChange(Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd,
		uint32_t newCwnd) {
	//NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t"/* << newCwnd*/);
	*stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << oldCwnd
			<< "\t" << newCwnd << std::endl;
}

static void TraceCwnd(std::string cwnd_tr_file_name, int i) {
	AsciiTraceHelper ascii;

	if (cwnd_tr_file_name.compare("") == 0) {
		NS_LOG_DEBUG ("No trace file for cwnd provided");
		return;
	} else {
		char numstr[21];
		sprintf(numstr, "%d", i);
		std::string cwnd_file = cwnd_tr_file_name + numstr;

		Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(
				cwnd_file.c_str());
		Config::ConnectWithoutContext(
				"/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
				MakeBoundCallback(&CwndChange, stream));
	}
}

/*static void MacDrop() {
 NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t");
 }*/

static void PhyDrop(Ptr<const Packet> packet) {
	//NS_LOG_UNCOND ("test");
	//counter++;
	NS_LOG_UNCOND("PhyTx at " << Simulator::Now ().GetSeconds () << " " << packet << " " << counter);
	//Ptr<Packet> packet1;
	//std::cout << *packet;

}

/*void PhyTxTrace(std::string context, Ptr<const Packet> packet, WifiMode mode,
 WifiPreamble preamble, uint8_t txPower) {
 std::cout << "PHYTX mode=" << mode << " " << *packet << std::endl;
 }*/

static void TraceDrops(/*std::string cwnd_tr_file_name*/) {
	/*AsciiTraceHelper ascii;

	 if (cwnd_tr_file_name.compare("") == 0) {
	 NS_LOG_DEBUG ("No trace file for cwnd provided");
	 return;
	 } else {
	 char numstr[21];
	 sprintf(numstr, "%d", i);
	 std::string cwnd_file = cwnd_tr_file_name + numstr;

	 Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(
	 cwnd_file.c_str());
	 Config::ConnectWithoutContext(
	 "/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow",
	 MakeBoundCallback(&CwndChange, stream));
	 }*/
	/*Config::ConnectWithoutContext(
	 "/NodeList/0/DeviceList/0/$ns3::WifiNetDevice/Mac/MacRxDrop",
	 MakeCallback(&MacDrop));*/

	Config::ConnectWithoutContext(
			"/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop",
			MakeCallback(&PhyDrop));

	/*Config::Connect("/NodeList/3/DeviceList/0/Phy/State/Tx",
	 MakeCallback(&PhyTxTrace));*/
}

/*static void RxDrop(Ptr<PcapFileWrapper> file, Ptr<const Packet> p) {
 NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
 file->Write(Simulator::Now(), p);
 }*/

int main(int argc, char *argv[]) {
	//StaticStuff ss;
	//Packet packet;
	//StaticStuff::print(packet);

	//std::cout<<"/ntest/n"<<std::endl;
	//StaticStuff::print();

	LogComponentEnable("SixthScriptExample", LOG_LEVEL_ALL);
 
        LogComponentEnable("WifiMacQueue", LOG_LEVEL_INFO);

	LogComponentEnable("TcpWestwood", LOG_LEVEL_ALL);

	//LogComponentEnable("BulkSendApplication", LOG_LEVEL_INFO);
	//LogComponentEnable("DropTailQueue", LOG_LEVEL_ALL);
	//LogComponentEnable("DcaTxop", LOG_LEVEL_FUNCTION);
	//WifiHelper::EnableLogComponents();

	uint32_t N = 2; //number of clients
	float duration = 10;
	double data_mbytes = 4;
	uint32_t mtu_bytes = 1500;
	std::string transport_prot = "TcpReno";
	std::string output_cwnd_file = "cwndSize";
	std::string output_pcap_file = "sixth.pcap";
	CommandLine cmd;

	cmd.AddValue("transport_prot",
			"Transport protocol to use: TcpTahoe, TcpReno, TcpNewReno, TcpWestwood, TcpWestwoodPlus ",
			transport_prot);
	cmd.AddValue("output_cwnd_file", "output cwnd file ", output_cwnd_file);
	cmd.AddValue("output_pcap_file", "output pcap file ", output_pcap_file);
	cmd.AddValue("nNodes", "number of clients ", N);
	cmd.AddValue("duration", "transmit duration ", duration);
	cmd.AddValue("data_mbytes", "data mbytes ", data_mbytes);
	cmd.Parse(argc, argv);

	// Set the simulation start and stop time
	float start_time = 0.1;
	float stop_time = start_time + duration;

	// Select TCP variant
	if (transport_prot.compare("TcpTahoe") == 0)
		Config::SetDefault("ns3::TcpL4Protocol::SocketType",
				TypeIdValue(TcpTahoe::GetTypeId()));
	else if (transport_prot.compare("TcpReno") == 0)
		Config::SetDefault("ns3::TcpL4Protocol::SocketType",
				TypeIdValue(TcpReno::GetTypeId()));
	else if (transport_prot.compare("TcpNewReno") == 0)
		Config::SetDefault("ns3::TcpL4Protocol::SocketType",
				TypeIdValue(TcpNewReno::GetTypeId()));
	else if (transport_prot.compare("TcpWestwood") == 0) {
		Config::SetDefault("ns3::TcpL4Protocol::SocketType",
				TypeIdValue(TcpWestwood::GetTypeId()));
		Config::SetDefault("ns3::TcpWestwood::FilterType",
				EnumValue(TcpWestwood::TUSTIN));
	} else if (transport_prot.compare("TcpWestwoodPlus") == 0) {
		Config::SetDefault("ns3::TcpL4Protocol::SocketType",
				TypeIdValue(TcpWestwood::GetTypeId()));
		Config::SetDefault("ns3::TcpWestwood::ProtocolType",
				EnumValue(TcpWestwood::WESTWOODPLUS));
		Config::SetDefault("ns3::TcpWestwood::FilterType",
				EnumValue(TcpWestwood::TUSTIN));
	} else {
		NS_LOG_DEBUG ("Invalid TCP version");
		exit(1);
	}

	// Calculate the ADU size
	Header* temp_header = new Ipv4Header();
	uint32_t ip_header = temp_header->GetSerializedSize();
	NS_LOG_LOGIC ("IP Header size is: " << ip_header);
	delete temp_header;
	temp_header = new TcpHeader();
	uint32_t tcp_header = temp_header->GetSerializedSize();
	NS_LOG_LOGIC ("TCP Header size is: " << tcp_header);
	delete temp_header;
	uint32_t tcp_adu_size = mtu_bytes - (ip_header + tcp_header);
	NS_LOG_LOGIC ("TCP ADU size is: " << tcp_adu_size);

	//Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold",	StringValue("2200"));
	Config::SetDefault("ns3::WifiMacQueue::MaxPacketNumber", UintegerValue(50));
	// Config::SetDefault("ns3::WifiMacQueue::MaxDelay", TimeValue(Seconds(1000)));

	//create nodes in a star.
	NS_LOG_INFO ("Create nodes.");
	NodeContainer wifiServerNode, wifiClientNodes;
	wifiServerNode.Create(1);
	wifiClientNodes.Create(N);

	/*NodeContainer nodes = NodeContainer();
	 nodes.Add(wifiServerNode);
	 nodes.Add(wifiClientNodes);

	 PointToPointHelper pointToPoint;
	 pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
	 pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

	 NetDeviceContainer devices;
	 devices = pointToPoint.Install (nodes);

	 Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
	 em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
	 devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

	 InternetStackHelper stack;
	 stack.Install (nodes);

	 Ipv4AddressHelper address;
	 address.SetBase ("10.1.1.0", "255.255.255.252");
	 Ipv4InterfaceContainer wifiServerInterfaces;
	 Ipv4InterfaceContainer wifiClientInterfaces;
	 wifiServerInterfaces = address.Assign( devices.Get (0));
	 wifiClientInterfaces = address.Assign( devices.Get (1));*/

	InternetStackHelper stack;
	stack.InstallAll();

	//create the channels
	NS_LOG_INFO ("Create channels.");

	WifiHelper wifi = WifiHelper::Default();
	wifi.SetStandard(WIFI_PHY_STANDARD_80211g);
	NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
	YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
	YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
	phy.SetChannel(channel.Create());

	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
			StringValue("ErpOfdmRate54Mbps"));

	/*wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
			StringValue("ErpOfdmRate54Mbps"), "ControlMode",
			StringValue("ErpOfdmRate54Mbps"), "RtsCtsThreshold",
			UintegerValue(10000), "IsLowLatency", BooleanValue(false),
			"NonUnicastMode", StringValue("ErpOfdmRate54Mbps"));*/

	//wifi.SetRemoteStationManager("ns3::CaraWifiManager");

	Ssid ssid = Ssid("ns-3-ssid");

	mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
	NetDeviceContainer serverDevice;
	serverDevice = wifi.Install(phy, mac, wifiServerNode);

	mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing",
			BooleanValue(false));
	NetDeviceContainer clientDevices;
	clientDevices = wifi.Install(phy, mac, wifiClientNodes);

	MobilityHelper mobility;
	std::ostringstream s;
	double r = 2.0;
	double u = 2 * 3.1415926 / N;
	double x = 0.0;
	double y = 0.0;
	Ptr<ListPositionAllocator> clientPositionAlloc = CreateObject<
			ListPositionAllocator>();
	Ptr<ListPositionAllocator> serverPositionAlloc = CreateObject<
			ListPositionAllocator>();

	for (uint32_t i = 0; i < N; i++) {
		x = r * std::cos(u * i);
		y = r * std::sin(u * i);
		clientPositionAlloc->Add(Vector(x, y, 0.0));
	}
	mobility.SetPositionAllocator(clientPositionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(wifiClientNodes);

	serverPositionAlloc->Add(Vector(0.0, 0.0, 0.0));
	mobility.SetPositionAllocator(serverPositionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(wifiServerNode);

	//add IP addresses.
	NS_LOG_INFO ("Assign IP Addresses.");
	Ipv4AddressHelper address;
	address.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer wifiServerInterfaces;
	Ipv4InterfaceContainer wifiClientInterfaces;
	wifiServerInterfaces = address.Assign(serverDevice);
	wifiClientInterfaces = address.Assign(clientDevices);

	//Turn on global static routing
	NS_LOG_INFO ("Initialize Global Routing.");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	uint16_t sinkPort = 8080;
	ApplicationContainer sourceApp;
	ApplicationContainer sinkApps;
	for (uint32_t i = 0; i < N; i++) {
		Address sinkAddress(
				InetSocketAddress(wifiClientInterfaces.GetAddress(i),
						sinkPort));

		Config::SetDefault("ns3::TcpSocket::SegmentSize",
				UintegerValue(tcp_adu_size));

		BulkSendHelper source("ns3::TcpSocketFactory", sinkAddress);
		source.SetAttribute("SendSize", UintegerValue(tcp_adu_size));
		source.SetAttribute("MaxBytes",
				UintegerValue(int(data_mbytes * 1000000)));
		sourceApp.Add(source.Install(wifiServerNode.Get(0)));

		PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",
				InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
		sinkApps.Add(packetSinkHelper.Install(wifiClientNodes.Get(i)));

		Simulator::Schedule(Seconds(start_time + 0.0001), &TraceCwnd,
				output_cwnd_file, i);

	}
	Simulator::Schedule(Seconds(start_time + 0.0001), &TraceDrops);

	/*PcapHelper pcapHelper;
	 Ptr<PcapFileWrapper> file = pcapHelper.CreateFile ("Phy.pcap", std::ios::out, PcapHelper::DLT_PPP);
	 wifiServerNode.Get (0)->TraceConnectWithoutContext("PhyRxDrop", MakeBoundCallback (&RxDrop, file));*/

	sourceApp.Start(Seconds(start_time));
	sourceApp.Stop(Seconds(stop_time));
	sinkApps.Start(Seconds(start_time));
	sinkApps.Stop(Seconds(stop_time));

	phy.EnablePcapAll(output_pcap_file);

	Simulator::Stop(Seconds(stop_time));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

