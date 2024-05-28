import pandas as pd
import matplotlib.pyplot as plt

# 读取结果文件
df = pd.read_csv("results.csv")

# 将dataRate和delay列转换为数值类型以便于绘图
print(df["dataRate"])
df["dataRate"] = df["dataRate"].str.replace("Kbps", "").replace("Mbps", "000").astype(int)
df["delay"] = df["delay"].str.replace("ms", "").astype(int)


# 定义绘图函数
def plot_metric(metric, ylabel):
    plt.figure(figsize=(10, 6))
    for key, grp in df.groupby(["maxPackets", "interval"]):
        plt.plot(
            grp["dataRate"],
            grp[metric],
            label=f"maxPackets={key[0]}, interval={key[1]}",
        )
    plt.xlabel("Data Rate (Kbps)")
    plt.ylabel(ylabel)
    plt.title(f"{ylabel} vs Data Rate")
    plt.legend(loc="best")
    plt.grid(True)
    plt.show()


# 绘制图表
plot_metric("Total_Tx_Packets", "Total Tx Packets")
plot_metric("Total_Rx_Packets", "Total Rx Packets")
plot_metric("Total_Lost_Packets", "Total Lost Packets")
plot_metric("Packet_Loss_Ratio", "Packet Loss Ratio")
plot_metric("Average_Delay", "Average Delay (s)")
plot_metric("Average_Jitter", "Average Jitter (s)")
