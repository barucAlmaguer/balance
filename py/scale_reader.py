from scale import *
import matplotlib.pyplot as plt
x_low = 0
x_high = 20
y_low = -0.5
y_high = 0.5
plt.axis([x_low, x_high, y_low, y_high])
plt.ion()

#init serial device
s = Scale()
s.initScale(0)
d = []
y_prom = []
center = s.getNext()
for i in range(100):
    if i >= x_high:
        x = [i for i in range(i - 20, i)]
        y = s.getNext() - center
        d.append(y)
        prom = sum(d)/len(d)
        d.pop(0)
        y_prom.pop(0)
        y_prom.append(prom)
        plt.axis([i - x_high, i, y_low, y_high])
        plt.scatter(x, d, color='b')
        plt.plot(x, y_prom, color='r')
    else:
        y = s.getNext() - center
        d.append(y)
        prom = sum(d)/len(d)
        y_prom.append(prom)
        plt.scatter(i, y, color='b')
        plt.plot(i, prom, color='r')
    plt.pause(0.01)

while True:
    plt.pause(0.01)