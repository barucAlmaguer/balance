from scale import *
import matplotlib.pyplot as plt
x_low = 0
x_high = 20
y_low = -0.5
y_high = 0.5
median_cnt = 10
plt.axis([x_low, x_high, y_low, y_high])
plt.ion()

#init serial device
s = Scale()
s.initScale(0)
d = []
y_prom = []
center = s.getNext()
i = 0
while True:
    if i >= x_high:
        x = [i for i in range(i - x_high, i)]
        y = s.getNext() - center
        d.append(y)
        d.pop(0)
        d2 = list(d[-median_cnt:])
        prom = sum(d2)/len(d2)
        y_prom.append(prom)
        y_prom.pop(0)
        x2 = x[-median_cnt:]
        y2 = list(y_prom[-median_cnt:])
        y_low = min(d) - 0.5
        y_high = max(d) + 0.5
        plt.axis([i - x_high, i, y_low, y_high])
        plt.scatter(x, d, color='b')
        plt.plot(x2, y2, color='r')
    else:
        y = s.getNext() - center
        d.append(y)
        prom = sum(d)/len(d)
        y_prom.append(prom)
        y_low = min(d) - 0.5
        y_high = max(d) + 0.5
        plt.axis([i - x_high, i, y_low, y_high])
        plt.scatter(i, y, color='b')
        plt.plot(i, prom, color='r')
    plt.pause(0.01)
    i += 1

while True:
    plt.pause(0.01)