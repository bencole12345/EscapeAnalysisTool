import sys
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from argparse import ArgumentParser
from pathlib import Path


# Set up argument parser
arg_parser = ArgumentParser(description="Plot graphs of the data")
arg_parser.add_argument("--dataDir", type=str, help="The directory containing the CSV files to plot")
arg_parser.add_argument("--outputDir", type=str, help="The directory to which the graphs should be written")
args = arg_parser.parse_args()

# Work out the input and output files
input_files_dir = Path(args.dataDir)
output_files_dir = Path(args.outputDir)

# List of files to process
# TODO: Update this with the actual programs we analysed
to_plot = [
    ("test_data_1.csv", "Fake Program 1"),
    ("test_data_2.csv", "Fake Program 2")
]

total_stack_allocations = []
total_escaping_stack_allocations = []
total_allocated_stack_memory = []
total_escaping_allocated_stack_memory = []

# Do the plot for each CSV file
for file_name, program_title in to_plot:
    
    # Read the CSV file
    csv_location = input_files_dir / file_name
    df = pd.read_csv(csv_location.resolve(), sep=";")

    # Add the data from this file
    total_stack_allocations.append(df["NumStackAllocations"].sum())
    total_escaping_stack_allocations.append(df["NumEscapingStackAllocations"].sum())
    total_allocated_stack_memory.append(df["TotalAllocatedStackMemory"].sum())
    total_escaping_allocated_stack_memory.append(df["TotalEscapingAllocateStackMemory"].sum())
    

labels = [data_source[1] for data_source in to_plot]
x = np.arange(len(labels))  # Label locations
width = 0.35                # Width of bars


# Plot "number of alloca() calls" graph

fig, ax = plt.subplots()
rects1 = ax.bar(x - width/2, total_stack_allocations, width, label="Total alloca() calls")
rects2 = ax.bar(x + width/2, total_escaping_stack_allocations, width, label="Escaping alloca() calls")

ax.set_ylabel("Count")
ax.set_title("Number of alloca calls in source code")
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend()

output_file = output_files_dir / "alloca_calls.pdf"
plt.savefig(output_file.resolve())

# Plot "size of allocations" graph

fig, ax = plt.subplots()
rects1 = ax.bar(x - width/2, total_allocated_stack_memory, width, label="Allocated stack memory")
rects2 = ax.bar(x + width/2, total_escaping_allocated_stack_memory, width, label="Allocated escaping stack memory")

ax.set_ylabel("Amount (bytes)")
ax.set_title("Amount of stack memory statically allocated")
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend()

output_file = output_files_dir / "amount_allocated.pdf"
plt.savefig(output_file.resolve())
