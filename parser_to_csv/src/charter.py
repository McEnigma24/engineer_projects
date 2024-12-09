import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Path to the "output" subfolder
folder_path = "./output"
# Path to save the charts
output_charts_folder = os.path.join(folder_path, "demo_charts")

# Create the "demo_charts" folder if it doesn't exist
if not os.path.exists(output_charts_folder):
    os.makedirs(output_charts_folder)

# Debug mode: Set to True to print data to terminal, False to suppress output
debug_mode = False

# Loop through all files in the "output" folder
for filename in os.listdir(folder_path):
    if filename.endswith(".csv"):
        # Construct full file path
        file_path = os.path.join(folder_path, filename)
        
        # Load the CSV file (read as plain text first)
        with open(file_path, 'r') as file:
            lines = file.readlines()

        data = []
        group_name = None
        headers = []
        expected_column_count = 0
        
        for line in lines:
            line = line.strip()
            
            # Check if the line starts with "GROUP" to save the current data
            if line.startswith("GROUP"):
                # If there is collected data, save it to a DataFrame and plot
                if data:
                    if all(len(row) == expected_column_count for row in data):
                        # Create a DataFrame for the current group
                        df = pd.DataFrame(data, columns=["X"] + headers)
                        
                        # Skip if the DataFrame is empty or invalid
                        if df.empty:
                            if debug_mode:
                                print(f"Warning: Empty or invalid data for group {group_name} in file {filename}. Skipping...")
                            data.clear()
                            continue
                        
                        # Plot the data
                        plt.figure(figsize=(10, 6))
                        for column in df.columns[1:]:
                            if df[column].notna().any():
                                plt.plot(df["X"], df[column] / 1_000_000, label=column)  # Convert to milliseconds

                        # Set plot title, labels, and legend
                        plot_title = f"{filename.split('.')[0]} - {group_name}"                        
                        plot_file = f"{filename.split('.')[0]}___{group_name}"
                        plot_file = plot_file.replace(',', '_').replace('-', '_').replace(' ', '_')
                        plt.title(plot_title)
                        plt.ylabel("Measured time [ms]")

                        # Set y-ticks to be evenly spaced
                        max_y_value = df.iloc[:, 1:].max().max() / 1_000_000
                        y_ticks = np.linspace(0, max_y_value, num=10)
                        plt.yticks(y_ticks)

                        plt.legend(title="Legend", bbox_to_anchor=(0.5, -0.1), loc='upper center', ncol=3)

                        # Save the plot to the demo_charts folder
                        save_path = os.path.join(output_charts_folder, f"{plot_file}.png")
                        plt.savefig(save_path, bbox_inches='tight')
                        plt.close()

                        data.clear()
                    else:
                        if debug_mode:
                            print(f"Warning: Collected data for {group_name} in file {filename} does not match expected column count ({expected_column_count} expected). Skipping...")
                        data.clear()

                # Update the current group name
                group_name = line.split(";")[1].strip() if len(line.split(";")) > 1 else None
                if debug_mode:
                    print(f"New Group: {group_name}")

                # Reset headers for the new group
                headers = []
                expected_column_count = 0
                continue

            # Process header line (contains column names)
            if not headers:
                headers = [header for header in line.split(";")[1:] if header]
                expected_column_count = len(headers) + 1  # +1 for the "X" column
                if debug_mode:
                    print(f"Headers for group {group_name}: {headers}")
                    print(f"Expected columns: {expected_column_count}")
                continue

            # Process normal data lines
            values = line.split(";")
            x_value = values[0].replace("'", "")  # Remove the thousands separator (')

            if x_value:
                # Convert y-values to float, skip empty and non-numeric values
                y_values = []
                for v in values[1:]:
                    try:
                        if v:
                            y_values.append(float(v))
                    except ValueError:
                        y_values.append(None)
                
                if y_values:
                    data.append([x_value] + y_values)

        # After finishing reading lines, check if there is data left to save
        if data and group_name:
            if all(len(row) == expected_column_count for row in data):
                df = pd.DataFrame(data, columns=["X"] + headers)

                plt.figure(figsize=(10, 6))
                for column in df.columns[1:]:
                    if df[column].notna().any():
                        plt.plot(df["X"], df[column] / 1_000_000, label=column)

                plot_title = f"{filename.split('.')[0]} - {group_name}"
                plot_file = f"{filename.split('.')[0]}___{group_name}"
                plot_file = plot_file.replace(',', '_').replace('-', '_').replace(' ', '_')
                plt.title(plot_title)
                plt.ylabel("Measured time [ms]")

                max_y_value = df.iloc[:, 1:].max().max() / 1_000_000
                y_ticks = np.linspace(0, max_y_value, num=10)
                plt.yticks(y_ticks)

                plt.legend(title="Legend", bbox_to_anchor=(0.5, -0.1), loc='upper center', ncol=3)

                save_path = os.path.join(output_charts_folder, f"{plot_file}.png")
                plt.savefig(save_path, bbox_inches='tight')
                plt.close()
            else:
                if debug_mode:
                    print(f"Warning: Collected data for {group_name} in file {filename} does not match expected column count. Skipping...")

        else:
            if debug_mode:
                print(f"No valid data found in file {filename}")
