import numpy as np
import argparse
from scipy import integrate
from scipy.ndimage import zoom
from scipy.special import erf
import warnings
import sys
from matplotlib import pyplot as plt

plt.style.use("ggplot")
plt.rcParams['axes.facecolor'] = '#f7f9fc'
plt.rcParams.update({'font.size': 14})

warnings.filterwarnings('ignore')

parser = argparse.ArgumentParser(
  description='Solve 3D Poisson Equation using Green\'s Function and plot potential countour lines in the plane z=0.\n'
    'Differential Equation: \u0394 V(x, y, z) = - \u03f1(x, y, z)',
  formatter_class=argparse.ArgumentDefaultsHelpFormatter
)

parser.add_argument('-s',  type=float, default=-1, help='x and y starting value')
parser.add_argument('-e',  type=float, default=1, help='x and y end value')
parser.add_argument('-r',  type=float, default=0.2, help='x and y resolution')
parser.add_argument('-i',  type=float, default=2, help='x, y and z integration radius')
parser.add_argument('-f',  type=float, default=4, help='Number of digits to print')
parser.add_argument('-c',  default='-np.exp(-(x**2+y**2+z**2)/0.5)/np.sqrt(np.pi)', type=str, help='Python Code for charge density \u03f1(x, y, z)')

args = parser.parse_args()
res, start, end, rad = args.r, args.s, args.e, args.i

if len(sys.argv) < 2:
  parser.print_help()
  print("\n\nIntegrating...\n[x]\t\t[y]\t\t[z]\t\t[V(x, y, z)]")

# Charge Density, defined in cli arguments
def charge(x, y, z):
  return eval(args.c)

# Green Function
def green(x, y, z, xp, yp, zp):
  dist = np.sqrt((x-xp)**2+(y-yp)**2+(z-zp)**2)
  return - 1/(4 * np.pi) / (dist + 1e-10) # adding 1e-10 at denominator prevents divergence

# Particular solution at point r
def _integral(x, y):
  def integrand(xp, yp, zp):
    return -green(x, y, 0, xp, yp, zp)*charge(xp, yp, zp)
  v = integrate.nquad(
    integrand,
    [[-rad, rad], [-rad, rad], [-rad, rad]],
    opts={"epsrel": 0.01, "epsabs": 0.001}
  )[0]
  print(f"{x:.4f}\t\t{y:.4f}\t\t0\t\t{v:.4f}")
  return v

# Vectorized version - avoids two for loops
integral = np.vectorize(_integral)

# Creation of the grid at z=0
xy = np.arange(start, end+res, res)
X, Y = np.meshgrid(xy, xy)

# Integration
V = integral(X, Y)

#np.save("pot.npy", V)
#V = np.load("pot.npy")

# Linear interpolation
V, X, Y = zoom(V, 10), zoom(X, 10), zoom(Y, 10)

# Plotting pseudocolor graphs for potential and charge density
fig, ax = plt.subplots(1, 2, figsize=(15, 7))
im = ax[0].imshow(V, extent=[start, end, start, end])
ax[0].set(xlabel="x", ylabel="y", title="Pseudocolor Plot for Potential")
fig.colorbar(im, ax=ax[0])
im = ax[1].imshow(charge(X, Y, 0), cmap="Reds", extent=[start, end, start, end])
ax[1].set(xlabel="x", ylabel="y", title="Charge Density")
fig.colorbar(im, ax=ax[1])
fig.savefig("1.pdf", dpi=100)
plt.show()

# Plotting countour lines for potential
fig, ax = plt.subplots(figsize=(10, 5))
cont = ax.contour(X, Y, V)
ax.clabel(cont)
ax.set(xlabel="x", ylabel="y", title="Contour Plot for Potential")
fig.savefig("2.pdf")
plt.show()

# Plotting analytical against numerical solution on the line y=z=0
one_D_x = np.arange(0, end, res*0.1)
one_D_v = integral(one_D_x, 0*one_D_x)
#np.save("1dpot.npy", one_D_v)
#one_D_v = np.load("1dpot.npy")

fig, ax = plt.subplots(figsize=(10, 5))
ax.plot(one_D_x, one_D_v, label="Numerical")
th_one_D_v = - np.sqrt(2)*erf(np.sqrt(2)*one_D_x)/(16*(one_D_x))
ax.plot(one_D_x, th_one_D_v, label="Analytical")
ax.legend()
ax.set(title="Numerical and Analitycal solution", xlabel="x", ylabel="y")
fig.savefig("3.pdf")
plt.show()
