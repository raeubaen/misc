import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv("dati.csv", sep=" ")
df.columns = ["perp", "z", "edep"]

_bins=[3000, 1000]
_range=[[0, 0.05], [-500, -495]]

means = np.histogram2d(df.perp, df.z, weights=df.edep, bins=_bins, range=_range)[0] / \
  np.histogram2d(df.perp, df.z, weights=df.edep, bins=_bins, range=_range)[0]

plt.imshow(np.nan_to_num(means))
plt.show()
