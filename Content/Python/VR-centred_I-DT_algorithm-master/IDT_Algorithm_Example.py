import pandas as pd

from IDT_alg_VR_centred import IDTVR

df_et = pd.read_csv("C:/Users/plust/Documents/Unreal Projects/TestETVisualize/Content/Python/VR-centred_I-DT_algorithm-master/Data/eyetracking.txt", sep=";")

print("Shape", df_et.shape)
print("Columns", df_et.columns)
print(df_et['elapsedTime'])

idt_vr = IDTVR()

df_et_res = idt_vr.fit_compute(df_et, time="time")
