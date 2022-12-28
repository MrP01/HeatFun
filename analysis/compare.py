import pathlib
import subprocess
import sys

import matplotlib.axes
import matplotlib.pyplot as plt
import numpy as np

BASE = pathlib.Path(__file__).parent.parent
RESULTS = BASE / "report" / "images"
sys.path.append(str(BASE / "build" / "lib"))
import heatfun  # type: ignore  # noqa

u0 = lambda x: np.exp(-(x**2) * 12)
x_to_plot = np.linspace(-1.0, 1.0, 500)


def heatfun_solution():
    cheb_x = heatfun.modifiedChebpoints(30)
    solution = heatfun.solve(u0(cheb_x), 0.01, x_to_plot)
    return solution


def matlab_solution():
    subprocess.run(["matlab", "-nodisplay", "-nosplash", "-nodesktop", "-r", "run('heatfun.m'); exit;"])
    solution = np.loadtxt(BASE / "analysis" / "matlab.csv", dtype=np.double)
    return solution


def analyse():
    our_solution = heatfun_solution()
    chebfun_solution = matlab_solution()
    print("Squared error:", sum((our_solution - chebfun_solution) ** 2))

    fig = plt.figure()
    axes: matplotlib.axes.Axes = fig.add_subplot(1, 1, 1)
    axes.plot(x_to_plot, u0(x_to_plot), label="$u_0(x)$")
    axes.plot(x_to_plot, our_solution, label="$u_{heatfun}(x, T)$")
    axes.plot(x_to_plot, chebfun_solution, label="$u_{chebfun}(x, T)$")
    axes.set_xlabel("$x$")
    axes.set_ylabel("$u(x)$")
    axes.legend()
    fig.savefig(str(RESULTS / "plot.png"))
    plt.show()


if __name__ == "__main__":
    analyse()
