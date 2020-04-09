# %%
"""
<a href="https://colab.research.google.com/github/raeubaen/giagu/blob/master/NB_PCA_tSNE_LC1_2020.ipynb" target="_parent"><img src="https://colab.research.google.com/assets/colab-badge.svg" alt="Open In Colab"/></a>
"""

# %%
"""
# Visualizzazione di dataset con PCA e tSNE
"""

# %%
"""
### Informazioni ###

scopo: impratichirsi con le operazioni di data-esploration utilizzando le tecniche di riduzione dimensionale PCA e tSNE.

**Il dataset MNIST**

Il dataset [MNIST](http://yann.lecun.com/exdb/mnist/) è uno dei dataset di bechmark più utlizzati nel ML. Prodotto da Yann LeCun che ha collezionato 70000 immagini di numeri scritti a mano (60000 per il training e 10000 per test). Ogni immagine è rappresentata da una matrice di 28x28 pixel oguno con una intensità nel range $[0,255]$ che rappresenta 256 variazioni del livello di grigio di quel pixel. 
"""

# %%
#import delle librerie necessarie
import time
import numpy as np


from sklearn.manifold import TSNE
#import seaborn as sns

# %%
"""
### Parte 1: load del dataset MNIST e data preprocessing

questo è disponibile vari frameowrk per ML noi utilizzeremo Tensorflow ...
"""

# %%
import tensorflow as tf
(x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data()

#convertiamo i dati in f32 e normalizziamo in [0,1] --> usiamo solo il campione di training in questo esempio
X = x_train.astype('float32')
X /= 255
y = y_train
print(X.shape, y.shape) #output 

# %%
# per comodità convertiamo le matrici che rappresentano i dati in undataframe di pandas
import pandas as pd

# rendiamo flat le matrici
X = np.reshape(X,(60000,28*28))

feat_cols = [ 'pixel'+str(i) for i in range(X.shape[1]) ]
df = pd.DataFrame(X,columns=feat_cols)
df['y'] = y
df['label'] = df['y'].apply(lambda i: str(i))
X, y = None, None
print('Size of the dataframe: {}'.format(df.shape))

# %%
df

# %%
# randomizziamo i dati (sempre buona cosa)

# For reproducability of the results
np.random.seed(1234)

rndperm = np.random.permutation(df.shape[0])

# %%
# Esempio di come con Pandas è semplice graficare un dataset ...
%matplotlib inline
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

plt.gray()
fig = plt.figure( figsize=(16,7) )
for i in range(0,15):
    ax = fig.add_subplot(3,5,i+1, title="Digit: {}".format(str(df.loc[rndperm[i],'label'])) )
    ax.matshow(df.loc[rndperm[i],feat_cols].values.reshape((28,28)).astype(float))
plt.show()

# %%
"""
### Parte 2: PCA 

utilizzeremo l'implementazione PCA disponibile dentro scikit-learn
"""

# %%
from sklearn.decomposition import PCA

#consideriamo solo le prime tre componenti
pca = PCA(n_components=3)
pca_result = pca.fit_transform(df[feat_cols].values)

df['pca-one'] = pca_result[:,0]
df['pca-two'] = pca_result[:,1] 
df['pca-three'] = pca_result[:,2]

print('Explained variation per principal component: {}'.format(pca.explained_variance_ratio_))

#NOTA: l'explained variation indica quanto ciascuna componente contribusice alla varianza totale del campione

# %%
df

# %%
#grafichiamo le prime due componenti
# per fare questo facciamo uno scatter plot delle due componenti e coloriamo ciaascun tipo di carattere con un 
# colore diverso ... questo può essere fatto direttamente usando Pandas!

ax = df.loc[rndperm,:].plot.scatter(x="pca-one", y="pca-two", c="y", colormap='tab10')

# %%
#3d usando matplotlib ...

ax = plt.figure(figsize=(16,10)).gca(projection='3d')
ax.scatter(
    xs=df.loc[rndperm,:]["pca-one"], 
    ys=df.loc[rndperm,:]["pca-two"], 
    zs=df.loc[rndperm,:]["pca-three"], 
    c=df.loc[rndperm,:]["y"], 
    cmap='tab10'
)
ax.set_xlabel('pca-one')
ax.set_ylabel('pca-two')
ax.set_zlabel('pca-three')
plt.show()

# %%
"""
### Parte 3: tSNE

utilizzeremo l'implementazione tSNE disponibile dentro scikit-learn

NOTA: tSNE è molto pesante dal punto di vista computazionale (CPU e RAM) con complessità che scala quadraticamente con il numero di eventi nel campione. L'applicabilità è limitata quindi a dataset di qualche migliaio di eventi, oppure bisogna prima di usarlo ridurre la dimensionalità con PCA (esempio $28\times 28 \to $ PCA $\to 25 \to$ tSNE, oppure usare meno campioni.

In questo caso usiamo tutte le 28x28 "features" ma usiamo solo 10k eventi.
"""

# %%
# selezioniamo i primi 10k eventi dal dataframe 
N = 10000
df_subset = df.loc[rndperm[:N],:].copy()
data_subset = df_subset[feat_cols].values

# calcoliamo di nuovo la PCA per confrontarci con tSNE
pca = PCA(n_components=3)
pca_result = pca.fit_transform(data_subset)
df_subset['pca-one'] = pca_result[:,0]
df_subset['pca-two'] = pca_result[:,1] 
df_subset['pca-three'] = pca_result[:,2]
print('Explained variation per principal component: {}'.format(pca.explained_variance_ratio_))

# %%
# tSNE

time_start = time.time()
tsne = TSNE(n_components=2, verbose=1, perplexity=40, n_iter=300)
tsne_results = tsne.fit_transform(data_subset)
print('t-SNE done! Time elapsed: {} seconds'.format(time.time()-time_start))

# %%
df_subset['tsne-2d-one'] = tsne_results[:,0]
df_subset['tsne-2d-two'] = tsne_results[:,1]

# %%
#confrontiamo i risultati PCA vs tSNE
plt.figure(figsize=(16,7))

ax1 = plt.subplot(1, 2, 1)
df_subset.plot.scatter(x="pca-one", y="pca-two", c="y", colormap='tab10', ax=ax1)

ax2 = plt.subplot(1, 2, 2)
df_subset.plot.scatter(x="tsne-2d-one", y="tsne-2d-two", c="y", colormap='tab10', ax=ax2)


# %%
#proviamo PCA + tSNE:

pca_50 = PCA(n_components=50)
pca_result_50 = pca_50.fit_transform(data_subset)
print('Cumulative explained variation for 50 principal components: {}'.format(np.sum(pca_50.explained_variance_ratio_)))

time_start = time.time()
tsne = TSNE(n_components=2, verbose=0, perplexity=40, n_iter=300)
tsne_pca_results = tsne.fit_transform(pca_result_50)
print('t-SNE done! Time elapsed: {} seconds'.format(time.time()-time_start))

# %%
df_subset['tsne-pca50-one'] = tsne_pca_results[:,0]
df_subset['tsne-pca50-two'] = tsne_pca_results[:,1]

ax4=df_subset.plot.scatter(x="tsne-pca50-one", y="tsne-pca50-two", c="y", colormap='tab10')

# %%
