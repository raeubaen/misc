import numpy as np
import argparse
from scipy import integrate
import sys

from matplotlib import pyplot as plt
plt.style.use("ggplot")
plt.rcParams['axes.facecolor'] = '#f7f9fc'

parser = argparse.ArgumentParser(
  description='Solve Damped Driven Harmonic Oscillator problem using Green\'s Function.\n'
    'Differential Equation: x\'\'(t) + 2 a x\'(t) + w0^2 x(t) = f(t)',
  formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument('-x0', type=float, default=0.1, help='Initial Position')
parser.add_argument('-v0', type=float, default=-0.1, help='Initial Velocity')
parser.add_argument('-w0', type=float, default=2, help='Non-damped pulsation')
parser.add_argument('-a',  type=float, default=0.1, help='Damping coefficient \'a\'')
parser.add_argument('-e',  type=float, default=15, help='Time end value')
parser.add_argument('-s',  type=float, default=0.1, help='Time step')
parser.add_argument('-p',  default='np.cos(t)', type=str, help='Python Code for driving function f(t)')

args = parser.parse_args()

if len(sys.argv) < 2:
  parser.print_help()

# Physical Parameters for equation x''(t) + 2 a x'(t) + w0^2 x(t) = f(t)
w0 = args.w0
a = args.a

# Initial Conditions at beginning of time
x0 = args.x0
v0 = args.v0

# Definition of time vector, with specified step
timestep = args.s
times = np.arange(0, args.e, timestep)

# Forcing signal
def pulse(t):
  return eval(args.p)

# Heaviside Theta
theta = lambda x: np.heaviside(x, 0)

# Homogeneous solution
def x_om(t):
  delta = np.sqrt(w0**2 - a**2) # discriminant of associated 2nd degree equation (imaginary part)
  x_om = np.exp(-a*t)*((v0 + a*x0)*np.sin(delta*t)/delta + x0*np.cos(delta*t))
  return x_om

# Green Function
def green(t, tp):
  delta = np.sqrt(w0**2 - a**2)
  dt = t - tp
  return theta(dt)*np.exp(-a*(dt))*np.sin(delta*(dt))/delta

# Particular solution at time t
def _integral(t):
  def integrand(tp):
    return green(t, tp)*pulse(tp)
  return integrate.quad(integrand, 0, 100)[0]

# Vectorized version
integral = np.vectorize(_integral)

def derivative(vector, timestep):
  return (vector[1:]-vector[:-1])/timestep

print("\n\nIntegrating the equation...\n")
sol_om = x_om(times) # vector with position at all times for homogeneous equation
sol = integral(times) + sol_om # vector with position at all times incuding particular solution

speed = derivative(sol, timestep)
v_om = derivative(sol_om, timestep)

# Plotting commands
fig, axes = plt.subplots(2, 1, sharex=True)

axes[0].plot(times, sol, color='black', label='Forced')
axes[0].plot(times, sol_om, color='black', linestyle='dashed', label='Homogeneous')
axes[0].grid(color="grey")
axes[0].set(ylabel="Position x(t)", xlabel="Time t")
axes[0].legend(loc="lower right")

axes[1].plot(times[:-1], speed, color='black', label='Forced')
axes[1].plot(times[:-1], v_om, color='black', linestyle='dashed', label='Homogeneous')
axes[1].grid(color="grey")
axes[1].set(ylabel="Velocity x'(t)", xlabel="Time t")
axes[1].legend(loc="lower right")

print("Plotting on file results.pdf")
fig.savefig("results.pdf")
print("Plotting on screen")
print("Close matplotlib figure to exit")
plt.show()
