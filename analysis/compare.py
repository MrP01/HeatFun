import pathlib
import sys

import matplotlib.pyplot as plt
import matplotlib.axes
import numpy as np

BASE = pathlib.Path(__file__).parent.parent
RESULTS = BASE / "report" / "images"
sys.path.append(str(BASE / "build" / "lib"))
import heatfun  # ignore


def analyse():
    u0 = lambda x: np.exp(-(x**2) * 12)
    cheb_x = heatfun.modifiedChebpoints(30)
    x_to_plot = np.linspace(-1.0, 1.0, 500)
    solution = heatfun.solve(u0(cheb_x), 0.01, x_to_plot)

    fig = plt.figure()
    axes: matplotlib.axes.Axes = fig.add_subplot(1, 1, 1)
    axes.plot(x_to_plot, u0(x_to_plot), label="$u_0(x)$")
    axes.plot(x_to_plot, solution, label="$u(x, T)$")
    axes.set_xlabel("$x$")
    axes.set_ylabel("$u(x)$")
    axes.legend()
    fig.savefig(str(RESULTS / "plot.png"))
    plt.show()


if __name__ == "__main__":
    analyse()
