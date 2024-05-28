import pandas as pd
import matplotlib.pyplot as plt

# 读取结果文件
df = pd.read_csv("results/results.csv", index_col=False)

# 将dataRate和delay列转换为数值类型以便于绘图
df["dataRate"] = df["dataRate"].str.replace("Kbps", "").astype(int)
df["delay"] = df["delay"].str.replace("ms", "").astype(int)


def preprocess_column(column, unit):
    if unit in column.iloc[0]:
        return column.str.replace(unit, "").astype(float)
    else:
        return column.astype(float)


df["Average_Delay"] = preprocess_column(df["Average_Delay"], "s")
df["Average_Jitter"] = preprocess_column(df["Average_Jitter"], "s")


# 定义绘图函数
def plot_metric(metric, ylabel):
    # 按 maxPackets 和 interval 分组
    for (maxPackets, interval, delay), grp in df.groupby(["maxPackets", "interval", "delay"]):
        plt.figure(figsize=(10, 6))
        for algorithm, data in grp.groupby("algorithm"):
            plt.plot(data["dataRate"], data[metric], label=f"algorithm={algorithm}")
        plt.xlabel("Data Rate (Kbps)")
        plt.ylabel(ylabel)
        plt.title(
            f"{ylabel} vs Data Rate\nmaxPackets={maxPackets}, interval={interval}, delay={delay}"
        )
        plt.legend(loc="best")
        plt.grid(True)
        # 保存图表，文件名包含 maxPackets 和 interval 信息
        plt.savefig(f"results/{metric}_maxPackets{maxPackets}_interval{interval}_delay{delay}.png")
        plt.close()


# 绘制图表
plot_metric("Packet_Loss_Ratio", "Packet Loss Ratio")
plot_metric("Average_Delay", "Average Delay (s)")
plot_metric("Average_Jitter", "Average Jitter (s)")
