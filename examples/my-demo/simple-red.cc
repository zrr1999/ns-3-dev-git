#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

void SetupTraffic(NodeContainer nodes, Ipv4InterfaceContainer interfaces) {
    // 创建一个应用：在节点1上安装一个UDP回显服务器
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // 创建一个应用：在节点0上安装一个UDP回显客户端，发送数据到节点1
    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(10));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
}

int main(int argc, char* argv[])
{
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // 默认队列管理器
    {
        std::cout << "Setting up topology with default queue manager..." << std::endl;
        NodeContainer nodes;
        nodes.Create(2);
        PointToPointHelper pointToPoint;
        pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
        pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
        NetDeviceContainer devices = pointToPoint.Install(nodes);
        InternetStackHelper stack;
        stack.Install(nodes);
        Ipv4AddressHelper address;
        address.SetBase("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces = address.Assign(devices);

        SetupTraffic(nodes, interfaces);

        std::cout << "Starting simulation with default queue manager..." << std::endl;
        Simulator::Run();
        std::cout << "Simulation with default queue manager finished." << std::endl;
        Simulator::Destroy();
    }

    // 使用RED队列管理器
    {
        std::cout << "Setting up topology with RED queue manager..." << std::endl;
        NodeContainer nodes;
        nodes.Create(2);
        PointToPointHelper pointToPoint;
        pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
        pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
        NetDeviceContainer devices = pointToPoint.Install(nodes);
        InternetStackHelper stack;
        stack.Install(nodes);
        Ipv4AddressHelper address;
        address.SetBase("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces = address.Assign(devices);

        // 删除默认队列管理
        TrafficControlHelper tchDelete;
        tchDelete.Uninstall(devices);

        // 创建RED队列管理
        TrafficControlHelper tchRed;
        tchRed.SetRootQueueDisc("ns3::RedQueueDisc");
        tchRed.Install(devices);

        SetupTraffic(nodes, interfaces);

        std::cout << "Starting simulation with RED queue manager..." << std::endl;
        Simulator::Run();
        std::cout << "Simulation with RED queue manager finished." << std::endl;
        Simulator::Destroy();
    }

    return 0;
}