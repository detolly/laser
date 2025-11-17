import sys
import matplotlib.pyplot as plt

x, y = [], []
for line in sys.stdin:
    parts = line.split(',')
    if len(parts) >= 2:
        x.append(float(parts[0]))
        y.append(float(parts[1]))

plt.plot(x, y, 'o')
plt.axis('equal')
plt.show()
