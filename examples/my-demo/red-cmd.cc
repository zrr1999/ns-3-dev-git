#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

void
SetupTraffic(NodeContainer nodes,
             Ipv4InterfaceContainer interfaces,
             uint32_t maxPackets,
             Time interval)
{
    // 创建一个应用：在节点1上安装一个UDP回显服务器
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(20.0));

    // 创建一个应用：在节点0上安装多个UDP回显客户端，发送数据到节点1
    for (int i = 0; i < 5; ++i)
    {
        UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
        echoClient.SetAttribute("MaxPackets", UintegerValue(maxPackets));
        echoClient.SetAttribute("Interval", TimeValue(interval));
        echoClient.SetAttribute("PacketSize", UintegerValue(1024));

        ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
        clientApps.Start(Seconds(2.0));
        clientApps.Stop(Seconds(20.0));
    }
}

void
PrintFlowMonitorStats(Ptr<FlowMonitor> flowMonitor, Ptr<Ipv4FlowClassifier> classifier)
{
    FlowMonitor::FlowStatsContainer stats = flowMonitor->GetFlowStats();
    uint32_t totalTxPackets = 0;
    uint32_t totalRxPackets = 0;
    uint32_t totalLostPackets = 0;
    double totalDelaySum = 0.0;
    double totalJitterSum = 0.0;
    uint64_t totalRxBytes = 0;

    for (auto iter = stats.begin(); iter != stats.end(); ++iter)
    {
        totalTxPackets += iter->second.txPackets;
        totalRxPackets += iter->second.rxPackets;
        totalLostPackets += iter->second.lostPackets;
        totalDelaySum += iter->second.delaySum.GetSeconds();
        totalJitterSum += iter->second.jitterSum.GetSeconds();
        totalRxBytes += iter->second.rxBytes;
    }

    double throughput = totalRxBytes * 8.0; 

    std::cout << "\nOverall Metrics:\n";
    std::cout << "  Total Tx Packets: " << totalTxPackets << "\n";
    std::cout << "  Total Rx Packets: " << totalRxPackets << "\n";
    std::cout << "  Total Lost Packets: " << totalLostPackets << "\n";
    std::cout << "  Overall Packet Loss Ratio: " << (double)totalLostPackets / totalTxPackets
              << "\n";
    std::cout << "  Overall Average Delay: "
              << (totalRxPackets > 0 ? totalDelaySum / totalRxPackets : 0) << "s\n";
    std::cout << "  Overall Average Jitter: "
              << (totalRxPackets > 1 ? totalJitterSum / (totalRxPackets - 1) : 0) << "s\n";
    std::cout << "  Overall Throughput: " << throughput << " bits\n";
}

int
main(int argc, char* argv[])
{
    uint32_t maxPackets = 1000;
    double intervalMs = 1.0;
    std::string dataRate = "20Mbps";
    std::string delay = "5ms";

    CommandLine cmd;
    cmd.AddValue("maxPackets", "Number of packets each client will send", maxPackets);
    cmd.AddValue("interval", "Interval between packets in milliseconds", intervalMs);
    cmd.AddValue("dataRate", "Data rate for the point-to-point link", dataRate);
    cmd.AddValue("delay", "Delay for the point-to-point link", delay);
    cmd.Parse(argc, argv);

    Time interval = MilliSeconds(intervalMs);

    // 默认队列管理器
    {
        std::cout << "Setting up topology with default queue manager..." << std::endl;
        NodeContainer nodes;
        nodes.Create(2);
        PointToPointHelper pointToPoint;
        pointToPoint.SetDeviceAttribute("DataRate", StringValue(dataRate));
        pointToPoint.SetChannelAttribute("Delay", StringValue(delay));
        NetDeviceContainer devices = pointToPoint.Install(nodes);
        InternetStackHelper stack;
        stack.Install(nodes);
        Ipv4AddressHelper address;
        address.SetBase("10.1.1.0", "255.255.255.0");
        Ipv4InterfaceContainer interfaces = address.Assign(devices);

        SetupTraffic(nodes, interfaces, maxPackets, interval);

        // 安装 FlowMonitor
        FlowMonitorHelper flowmonHelper;
        Ptr<FlowMonitor> flowMonitor = flowmonHelper.InstallAll();
        Ptr<Ipv4FlowClassifier> classifier =
            DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());

        std::cout << "Starting simulation with default queue manager..." << std::endl;
        Simulator::Stop(Seconds(22.0));
        Simulator::Run();

        // 打印 FlowMonitor 统计结果
        std::cout << "Simulation with default queue manager finished." << std::endl;
        PrintFlowMonitorStats(flowMonitor, classifier);
        Simulator::Destroy();
    }

    std::cout << std::endl;

    // 使用RED队列管理器
    {
        std::cout << "Setting up topology with RED queue manager..." << std::endl;
        NodeContainer nodes;
        nodes.Create(2);
        PointToPointHelper pointToPoint;
        pointToPoint.SetDeviceAttribute("DataRate", StringValue(dataRate));
        pointToPoint.SetChannelAttribute("Delay", StringValue(delay));
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

        SetupTraffic(nodes, interfaces, maxPackets, interval);

        // 安装 FlowMonitor
        FlowMonitorHelper flowmonHelper;
        Ptr<FlowMonitor> flowMonitor = flowmonHelper.InstallAll();
        Ptr<Ipv4FlowClassifier> classifier =
            DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());

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