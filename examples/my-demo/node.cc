#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <iostream>

using namespace ns3;

int main(int argc, char *argv[])
{
    // 启用日志记录
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // 创建两个节点
    NodeContainer nodes;
    nodes.Create(2);
    std::cout << "Created " << nodes.GetN() << " nodes." << std::endl;

    // 创建点对点连接
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
    std::cout << "Point-to-Point link with DataRate=5Mbps and Delay=2ms." << std::endl;

    // 在两个节点之间安装设备
    NetDeviceContainer devices = pointToPoint.Install(nodes);
    std::cout << "Installed devices on nodes. Total devices: " << devices.GetN() << std::endl;

    // 安装Internet协议栈
    InternetStackHelper stack;
    stack.Install(nodes);
    std::cout << "Installed internet stack on nodes." << std::endl;

    // 分配IP地址
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    std::cout << "Assigned IP addresses to devices. Total interfaces: " << interfaces.GetN() << std::endl;

    // 在节点1上安装一个UDP回显服务器
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));
    std::cout << "Installed UDP Echo Server on node 1, running from 1s to 10s." << std::endl;

    // 在节点0上安装一个UDP回显客户端，发送数据到节点1
    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(2048));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));
    std::cout << "Installed UDP Echo Client on node 0, sending data to node 1 from 2s to 10s." << std::endl;

    // 运行模拟器
    std::cout << "Starting simulation..." << std::endl;
    Simulator::Run();
    std::cout << "Simulation finished." << std::endl;
    Simulator::Destroy();

    return 0;
}