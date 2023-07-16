import pandas as pd
import sys

def load_df_from_csv(csv_file: str):
    df = pd.read_csv(csv_file, sep=",")
    df = df.rename(columns={'avg_exec_time' : f'version_{df.get("version")[0]}'})
    return df

if __name__ == '__main__':
    dataframes = []
    for path in sys.argv[1:]:
        dataframes.append(load_df_from_csv(path))
    if len(sys.argv) == 2:
        df = dataframes[0]
        res = df.plot(x='pixelNum', y=[f'version_{df.get("version")[0]}'], style='.-', title="Performance Test", xlabel="Pixel", ylabel="Execution time (s)").get_figure()
        res.savefig("sobel_graysc_version_4_5_only.png")
    else:
        df = dataframes[0]
        ax = df.plot(x='pixelNum', y=[f'version_{df.get("version")[0]}'], style='.-', title="Performance Test", xlabel="Pixel", ylabel="Execution time (s)") 
        for df in dataframes[1:]:
            df.plot(ax=ax, x='pixelNum', y=[f'version_{df.get("version")[0]}'], style='.-')
        
        ax.get_figure().savefig("sobel_graysc_version_4_5_only.png")
