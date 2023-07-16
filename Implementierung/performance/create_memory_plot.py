import pandas as pd
import sys

def load_df_from_csv(csv_file: str):
    df = pd.read_csv(csv_file, sep=",")
    return df

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Wrong amount of arguments. Please provide a csv file.")
        exit(1)

    df = load_df_from_csv(sys.argv[1])
    if len(sys.argv) == 2:
        res = df.plot(x='pixelNum', y=['avg_exec_time', 'avg_io_write_time', 'avg_io_read_time'], style='.-', title=f"Performance Version {df.get('version')[0]}", xlabel="Pixel", ylabel="Execution time (s)").get_figure()
        res.savefig("memory_plot.png")