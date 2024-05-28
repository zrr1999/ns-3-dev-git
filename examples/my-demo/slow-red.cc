#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/flow-monitor.h"

using namespace ns3;

void SetupTraffic(NodeContainer nodes, Ipv4InterfaceContainer interfaces) {
    // 创建一个应用：在节点1上安装一个UDP回显服务器
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(20.0));

    // 创建一个应用：在节点0上安装多个UDP回显客户端，发送数据到节点1
    for (int i = 0; i < 5; ++i) {
        UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
        echoClient.SetAttribute("MaxPackets", UintegerValue(2000)); // 增加发送的数据包数量
        echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(5))); // 增加发送频率
        echoClient.SetAttribute("PacketSize", UintegerValue(1024));

        ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
        clientApps.Start(Seconds(2.0));
        clientApps.Stop(Seconds(20.0));
    }
}

void PrintFlowMonitorStats(Ptr<FlowMonitor> flowMonitor, Ptr<Ipv4FlowClassifier> classifier) {
    FlowMonitor::FlowStatsContainer stats = flowMonitor->GetFlowStats();
    for (auto iter = stats.begin(); iter != stats.end(); ++iter) {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);
        std::cout << "Flow ID: " << iter->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
        std::cout << "  Tx Packets: " << iter->second.txPackets << "\n";
        std::cout << "  Tx Bytes:   " << iter->second.txBytes << "\n";
        std::cout << "  Rx Packets: " << iter->second.rxPackets << "\n";
        std::cout << "  Rx Bytes:   " << iter->second.rxBytes << "\n";
        std::cout << "  Lost Packets: " << iter->second.lostPackets << "\n";
        std::cout << "  Packet Loss Ratio: " << (double)iter->second.lostPackets / iter->second.txPackets << "\n";
        std::cout << "  Delay Sum: " << iter->second.delaySum.GetSeconds() << "s\n";
        std::cout << "  Average Delay: " << (iter->second.rxPackets > 0 ? iter->second.delaySum.GetSeconds() / iter->second.rxPackets : 0) << "s\n";
        std::cout << "  Jitter Sum: " << iter->second.jitterSum.GetSeconds() << "s\n";
        std::cout << "  Average Jitter: " << (iter->second.rxPackets > 1 ? iter->second.jitterSum.GetSeconds() / (iter->second.rxPackets - 1) : 0) << "s\n";
    }
}

int main(int argc, char* argv[])
{
    // 默认队列管理器
    {
        std::cout << "Setting up topology with default queue manager..." << std::endl;
        NodeContainer nodes;
        nodes.Create(2);
        PointToPointHelper pointToPoint;
        pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Mbps")); // 降低带宽
        pointToPoint.SetChannelAttribute("Delay", StringValue("10ms")); // 增加延迟
        NetDeviceContainer devices = pointToPoint.Install(nodes);
        InternetStackHelper stack;
        stack.Install(nodes);
        Ipv4AddressHelper address;
        address.SetBase("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces = address.Assign(devices);

        SetupTraffic(nodes, interfaces);

        // 安装 FlowMonitor
        FlowMonitorHelper flowmonHelper;
        Ptr<FlowMonitor> flowMonitor = flowmonHelper.InstallAll();
        Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());

        std::cout << "Starting simulation with default queue manager..." << std::endl;
        Simulator::Stop(Seconds(22.0));
        Simulator::Run();

        // 打印 FlowMonitor 统计结果
        std::cout << "Simulation with default queue manager finished." << std::endl;
        PrintFlowMonitorStats(flowMonitor, classifier);
        Simulator::Destroy();
    }

    // 使用RED队列管理器
    {
        std::cout << "Setting up topology with RED queue manager..." << std::endl;
        NodeContainer nodes;
        nodes.Create(2);
        PointToPointHelper pointToPoint;
        pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Mbps")); // 降低带宽
        pointToPoint.SetChannelAttribute("Delay", StringValue("10ms")); // 增加延迟
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

        // 安装 FlowMonitor
        FlowMonitorHelper flowmonHelper;
        Ptr<FlowMonitor> flowMonitor = flowmonHelper.InstallAll();
        Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());

        std::cout << "Starting simulation with RED queue manager..." << std::endl;
        Simulator::Stop(Seconds(22.0));
        Simulator::Run();

        // 打印 FlowMonitor 统计结果
        std::cout << "Simulation with RED queue manager finished." << std::endl;
        PrintFlowMonitorStats(flowMonitor, classifier);
        Simulator::Destroy();
    }

    return 0;
}