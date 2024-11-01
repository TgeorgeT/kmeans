from sklearn.datasets import make_blobs
import pandas as pd
import numpy as np

X, _ = make_blobs(n_samples=1000000, centers=5, random_state=100, center_box=(0, 100))

X_min = X.min(axis=0)
X -= X_min
X_max = X.max(axis=0)

X = X * (100 / X_max)

data = pd.DataFrame(X, columns=["X", "Y"])
data.to_csv("./data/kmeans_data_set.csv", index=False)
from sklearn.datasets import make_blobs
import pandas as pd

X, _ = make_blobs(n_samples=200, centers=5, random_state=100, center_box=(0, 10))

data = pd.DataFrame(X, columns=["X", "Y"])

data.to_csv("./data/kmeans_data_set1.csv", index=False)
