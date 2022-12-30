import pathlib
import subprocess
import sys

import matplotlib.axes
import matplotlib.pyplot as plt
import numpy as np
import tikzplotlib

BASE = pathlib.Path(__file__).parent.parent
RESULTS = BASE / "report" / "figures"
sys.path.append(str(BASE / "build" / "lib"))
import heatfun  # type: ignore  # noqa

INTERESTING_U0 = {
    "gaussian": "exp(-12 * x**2)",
    "sines": "sin((4*x)^2) + sin(4*x)^2",
    "radiation": "1/(x^2+0.05)",
    "tanh-kernel": "tanh(-10*x^4)",
    "double-tanh": "tanh(10*(x+0.5))-tanh(10*(x-0.5))",
}
X_TO_PLOT = np.linspace(-1.0, 1.0, 500)

# plt.style.use("ggplot")


def heatfun_solution(example_key: str):
    cheb_x = heatfun.modifiedChebpoints(30)
    cheb_y = heatfun.evaluateExpression(INTERESTING_U0[example_key], cheb_x)
    solution = heatfun.solve(cheb_y, 0.01, X_TO_PLOT)
    return solution


def matlab_solution(reevaluate=False):
    if reevaluate:
        subprocess.run(["matlab", "-nodisplay", "-nosplash", "-nodesktop", "-r", "run('heatfun.m'); exit;"])
    solution = np.loadtxt(BASE / "analysis" / "matlab.csv", dtype=np.double)
    return solution


def analyse(example_key: str):
    our_solution = heatfun_solution(example_key)
    chebfun_solution = matlab_solution()
    print("Squared error:", sum((our_solution - chebfun_solution) ** 2))

    fig = plt.figure()
    axes: matplotlib.axes.Axes = fig.add_subplot(2, 1, 1)
    axes.plot(X_TO_PLOT, heatfun.evaluateExpression(INTERESTING_U0[example_key], X_TO_PLOT), label="$u(x, 0) = u_0(x)$")
    axes.plot(X_TO_PLOT, our_solution, label="$u_{heatfun}(x, T)$")
    axes.plot(X_TO_PLOT, chebfun_solution, label="$u_{chebfun}(x, T)$")
    axes.set_ylabel("$u(x)$")
    axes.legend()
    axes: matplotlib.axes.Axes = fig.add_subplot(2, 1, 2)
    axes.semilogy(X_TO_PLOT, abs(our_solution - chebfun_solution), "m")
    axes.set_xlabel("$x$")
    axes.set_ylabel("$|u_{heat}(x, T) - u_{cheb}(x, T)|$")
    tikzplotlib.save(
        RESULTS / "numerical-comparison.tex",
        figure=fig,
        axis_width=r"0.8\linewidth",
        axis_height=r"0.4\linewidth",
    )
    fig.savefig(str(RESULTS / "numerical-comparison.png"))
    plt.show()


if __name__ == "__main__":
    analyse("gaussian")
