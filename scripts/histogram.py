import json
import sys
import matplotlib.pyplot as plt

def extract_test_setup(json_file_path):
    with open(json_file_path, 'r') as file:
        data = json.load(file)


    # Extract and plot latency histogram
    latency_histogram = data.get("latency_histogram", [])
    latencies = []
    counts = []
    bar_colors = []
    for entry in latency_histogram:
        if entry["latency"] != "lost":
            latencies.append(str(entry["latency"]))
            counts.append(entry["count"])
            bar_colors.append((0.37, 0.52, 0.73)) 
        else:
            latencies.append("lost")
            counts.append(entry["count"])
            bar_colors.append((0.86, 0.36, 0.36)) 

    # Add parameters as text on the graph
    plt.figure(figsize=(10, 6))
    plt.xlabel("Latency (s)")
    plt.ylabel("Count")
    plt.grid(True, linestyle='--', axis='y', zorder=0)
    plt.bar(latencies, counts, color=bar_colors, width=0.9, zorder=3)
    if len(latencies) > 30:
        plt.xticks(ticks=range(0, len(latencies), 5), labels=[latencies[i] for i in range(0, len(latencies), 5)])
    else:
        plt.xticks(ticks=range(len(latencies)), labels=latencies)
    plt.savefig("histogram.png")

# Example usage
if len(sys.argv) != 2:
    print("Usage: python histogram.py <json_file_path>")
else:
    json_file_path = sys.argv[1]
    extract_test_setup(json_file_path)