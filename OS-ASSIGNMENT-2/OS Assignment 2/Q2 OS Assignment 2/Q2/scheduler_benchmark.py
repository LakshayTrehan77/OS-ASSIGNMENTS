import subprocess
import matplotlib.pyplot as plt

def read_execution_times(filename):
    execution_times = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        for line in lines:
            try:
                time = float(line.strip())  # Read execution times as float values
                execution_times.append(time)
            except ValueError:
                print(f"Skipping invalid line: {line.strip()}")
    return execution_times

def read_scheduling_policies(filename):
    policies = []
    with open(filename, 'r') as file:
        lines = file.readlines()
        for line in lines:
            policy = line.strip()
            policies.append(policy)
    return policies

def benchmark_scheduling_policies():
    policies = read_scheduling_policies("policies.txt")
    colors = ['r', 'g', 'b']
    execution_times = read_execution_times("execution_time.txt")  # Read execution times from the file

    # Check if the number of execution times matches the number of policies
    if len(execution_times) != len(policies):
        print("Error: Number of execution times does not match the number of policies.")
        return

    # Plot a bar graph with black borders
    plt.bar(policies, execution_times, color=colors, edgecolor='black', linewidth=1)
    plt.xlabel("Scheduling Policy", fontweight='bold')  # Make X-axis label bold
    plt.ylabel("Execution Time (seconds)")
    plt.title("Process Execution Time vs. Scheduling Policy", fontweight='bold')  # Make title bold

    # Add annotations (execution times) to the top of each bar with 5 decimal points
    for i, time in enumerate(execution_times):
        plt.text(i, time, f"{time:.5f}", ha='center', va='bottom', fontweight='bold')

    plt.xticks(rotation=0)  # Keep X-axis labels straight (not slanted)
    plt.show()

if __name__ == "__main__":
    benchmark_scheduling_policies()
