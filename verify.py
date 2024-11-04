import pandas as pd
from sklearn.cluster import KMeans
from sklearn.metrics import accuracy_score, adjusted_rand_score
import numpy as np

# Load data from CSV file
data = pd.read_csv("data/kmeans_result.csv")

# Extract the X and Y coordinates
X = data[["X", "Y"]]

# Set the number of clusters (assuming it matches the maximum cluster number in the CSV)
n_clusters = data["cluster"].nunique()

# Run k-means clustering
kmeans = KMeans(n_clusters=n_clusters, random_state=42)
data["predicted_cluster"] = kmeans.fit_predict(X)

# Map clusters to the closest original labels using a matching algorithm
# This approach aligns predicted clusters with original clusters for comparison


def map_clusters(true_labels, predicted_labels):
    from scipy.optimize import linear_sum_assignment
    from sklearn.metrics import confusion_matrix

    # Compute the confusion matrix between true and predicted labels
    matrix = confusion_matrix(true_labels, predicted_labels)
    # Use the Hungarian algorithm to find the best cluster matching
    row_ind, col_ind = linear_sum_assignment(-matrix)
    # Create a mapping based on this best assignment
    return {col: row for row, col in zip(row_ind, col_ind)}


# Map predicted clusters to the original clusters
mapping = map_clusters(data["cluster"], data["predicted_cluster"])
data["mapped_cluster"] = data["predicted_cluster"].map(mapping)

# Calculate accuracy and adjusted Rand index
accuracy = accuracy_score(data["cluster"], data["mapped_cluster"])
ari = adjusted_rand_score(data["cluster"], data["mapped_cluster"])

# Output results
print("Cluster Mapping:", mapping)
print("Accuracy:", accuracy)
print("Adjusted Rand Index:", ari)
print(
    "\nData with Predicted Clusters:\n", data[["X", "Y", "cluster", "mapped_cluster"]]
)
