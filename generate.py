from sklearn.datasets import make_blobs
import pandas as pd

X, _ = make_blobs(n_samples=100, centers=5, random_state=101)

data = pd.DataFrame(X, columns=["X", "Y"])

data.to_csv("./data/kmeans_training_set.csv", index=False)
