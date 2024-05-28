#!/bin/bash

# 定义参数的不同数值
maxPackets_values=(1000 10000 100000)
interval_values=(1)
dataRate_values=("100Kbps" "200Kbps" "400Kbps" "800Kbps" "1600Kbps" "3200Kbps" "6400Kbps" "12800Kbps" "25600Kbps" "51200Kbps")
delay_values=("1ms" "1000ms")

# 清空或创建结果文件
output_file="results/results.csv"
echo "maxPackets,interval,dataRate,delay,algorithm,Total_Tx_Packets,Total_Rx_Packets,Total_Lost_Packets,Packet_Loss_Ratio,Average_Delay,Average_Jitter" > $output_file

# 循环遍历所有参数组合
for maxPackets in "${maxPackets_values[@]}"; do
  for interval in "${interval_values[@]}"; do
    for dataRate in "${dataRate_values[@]}"; do
      for delay in "${delay_values[@]}"; do
        command="/home/zrr/workspace/ns-3-dev-git/build/examples/my-demo/ns3-dev-red-cmd-default --maxPackets=$maxPackets --interval=$interval --dataRate=$dataRate --delay=$delay"
        echo "Running command: $command"
        output=$($command)
        echo "Finished running command"
        
        # 提取两组输出信息
        output1=$(echo "$output" | grep -A 6 "Overall Metrics:" | head -n 7)
        output2=$(echo "$output" | grep -A 6 "Overall Metrics:" | tail -n 7)

        # 提取第一组信息
        tx_packets1=$(echo "$output1" | grep "Total Tx Packets" | awk '{print $4}')
        rx_packets1=$(echo "$output1" | grep "Total Rx Packets" | awk '{print $4}')
        lost_packets1=$(echo "$output1" | grep "Total Lost Packets" | awk '{print $4}')
        packet_loss_ratio1=$(echo "$output1" | grep "Overall Packet Loss Ratio" | awk '{print $5}')
        avg_delay1=$(echo "$output1" | grep "Overall Average Delay" | awk '{print $4}')
        avg_jitter1=$(echo "$output1" | grep "Overall Average Jitter" | awk '{print $4}')

        # 提取第二组信息
        tx_packets2=$(echo "$output2" | grep "Total Tx Packets" | awk '{print $4}')
        rx_packets2=$(echo "$output2" | grep "Total Rx Packets" | awk '{print $4}')
        lost_packets2=$(echo "$output2" | grep "Total Lost Packets" | awk '{print $4}')
        packet_loss_ratio2=$(echo "$output2" | grep "Overall Packet Loss Ratio" | awk '{print $5}')
        avg_delay2=$(echo "$output2" | grep "Overall Average Delay" | awk '{print $4}')
        avg_jitter2=$(echo "$output2" | grep "Overall Average Jitter" | awk '{print $4}')
        output1="$maxPackets,$interval,$dataRate,$delay,FIFO,$tx_packets1,$rx_packets1,$lost_packets1,$packet_loss_ratio1,$avg_delay1,$avg_jitter1"
        output2="$maxPackets,$interval,$dataRate,$delay,RED,$tx_packets2,$rx_packets2,$lost_packets2,$packet_loss_ratio2,$avg_delay2,$avg_jitter2"
        # 写入结果文件
        echo "$output1"
        echo "$output1" >> $output_file
        echo "$output2"
        echo "$output2" >> $output_file
      done
    done
  done
done