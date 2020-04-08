#!/usr/bin/env python
# coding: utf-8

# In[2]:


from audio2numpy import open_audio
import numpy as np
import matplotlib.pyplot as plt


# In[3]:


signal, signal_rate = open_audio("lisergic.mp3")


# In[137]:


orig = signal[:, 0]
L = len(orig)
orig_rms = np.sqrt(np.correlate(orig, orig))
noisies = []
for i in range(3):
  noisy = np.zeros(L)
  rnd = int(np.random.uniform()*L*0.01)
  print(rnd)
  noisy[0 : L-rnd] = orig[rnd : ]
  noisy[L - rnd : ] = orig[ : rnd]
  noisy += 1/orig_rms*np.random.uniform(size=len(orig))
  noisy_rms = np.sqrt(np.correlate(noisy, noisy))
  noisy_norm = noisy/noisy_rms
  noisies.append(noisy_norm)
orig = orig/orig_rms


# In[138]:


fig, a = plt.subplots(2)
a[0].plot(orig[:10000])
a[1].plot(noisies[0][:10000])
plt.show()


# In[158]:


def fase(x, _n):
  L = len(x)
  ret = []
  n = (-int(_n))%L
  y = np.zeros(L)
  y[0 : L-n] = x[n : ]
  y[L - n : ] = x[ : n]
  return y


# In[159]:


def der(rif, test, n):
    L = len(rif)
    A = int(np.random.uniform()*L/2)
    B = L - int(np.random.uniform()*L/2)
    if B >= L - 2:
        B = L - 3
    f = fase(test, n)
    der = rif[B+1]*f[B+1] + rif[B]*f[B] - rif[A-1]*f[A-1] - rif[A]*f[A]
    return -der

def gradient(rif, test_m, n_v):
    gd = []
    for i in range(len(n_v)):
        d = der(rif, test_m[i], n_v[i])
        gd.append(d/np.linalg.norm(d))
    return np.asarray(gd).reshape(3,)


# In[ ]:


v = np.zeros(3)
m = np.zeros(3)
beta_1 = 0.9
beta_2 = 0.9
w = np.ones(3)
step_size = 10
epsilon = 1e-8

for t in range(1, 1000):
    g = gradient(orig, noisies, w)
    m = beta_1 * m + (1 - beta_1) * g
    v = beta_2 * v + (1 - beta_2) * np.power(g, 2)
    m_hat = m / (1 - np.power(beta_1, t))
    v_hat = v / (1 - np.power(beta_2, t))
    w = w - step_size * m_hat / (np.sqrt(v_hat) + epsilon)
    print(w)

# In[ ]:


w


# In[ ]:




