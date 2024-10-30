import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("data/kmeans_result.csv")

plt.figure(figsize=(8, 6))
scatter = plt.scatter(
    data["X"], data["Y"], c=data["cluster"], cmap="viridis", s=100, alpha=0.7
)

plt.colorbar(scatter, label="Cluster")

plt.xlabel("X")
plt.ylabel("Y")
plt.title("Points by Cluster")

plt.show()
