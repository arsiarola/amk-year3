from statistics import mean
import matplotlib.pyplot as plt
import math
def dirac(num): # &
	return 1 if num == 0 else 0

def unit(num): # µ
	return 1 if num >= 0 else 0

def a(n):
	return dirac(n-2) + dirac(n) - 2*dirac(n-1)
def b(n):
	return unit(n+2) - unit(n-2)
def c(n):
	return math.cos(0.1 * math.pi * n)
def d(n):
	return math.cos(2 * math.pi * n)


#
time = list(range(-20, 20))
resultA = [a(n) for n in time]
resultB = [b(n) for n in time]
resultC = [c(n) for n in time]
resultD = [d(n) for n in time]

print(time)
print(resultA)
print(resultB)
print(resultC)
print(resultD)

fig, axs = plt.subplots(2, 2)
axs[0,0].plot(time, resultA)
axs[0,0].set_title("a")
axs[0,1].plot(time, resultB)
axs[0,1].set_title("b")
axs[1,0].plot(time, resultC)
axs[1,0].set_title("c")
axs[1,1].plot(time, resultD)
axs[1,1].set_title("d")

for ax in axs.flat:
	ax.set(xlabel="time", ylabel="result")

for ax in axs.flat:
	    ax.label_outer()

plt.show()
