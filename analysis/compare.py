import pathlib
import subprocess
import sys

import matplotlib.axes
import matplotlib.pyplot as plt
import numpy as np
import tikzplotlib


def monkeypatch_matplotlib():
    from matplotlib.lines import Line2D
    from matplotlib.legend import Legend

    Line2D._us_dashSeq = property(lambda self: self._dash_pattern[1])
    Line2D._us_dashOffset = property(lambda self: self._dash_pattern[0])
    Legend._ncol = property(lambda self: self._ncols)


monkeypatch_matplotlib()

BASE = pathlib.Path(__file__).parent.parent
RESULTS = BASE / "report" / "figures"
sys.path.append(str(BASE / "build" / "lib"))
import heatfun  # type: ignore  # noqa

INTERESTING_U0 = {
    "gaussian": "exp(-12*x^2)",
    "sines": "sin((4*x)^2) + sin(4*x)^2",
    "radiation": "1/(x^2+0.05)",
    "tanh-kernel": "tanh(-10*x^4)",
    "double-tanh": "tanh(10*(x+0.5))-tanh(10*(x-0.5))",
}
X_TO_PLOT = np.linspace(-1.0, 1.0, 500)
ORDER_OF_INTERPOLATION = 30

plt.style.use("seaborn-v0_8-muted")
STYLE_COLORS = list(plt.rcParams["axes.prop_cycle"])


def heatfun_solution(example_key: str):
    cheb_x = heatfun.modifiedChebpoints(ORDER_OF_INTERPOLATION)
    cheb_y = heatfun.evaluateExpression(INTERESTING_U0[example_key], cheb_x)
    solution = heatfun.solve(cheb_y, 0.01, X_TO_PLOT)
    return solution


def all_matlab_solutions():
    command = ""
    for key, func in INTERESTING_U0.items():
        command += (
            f"filename = 'matlab-result-{key}.csv'; func = '{func}'; disp(func); "
            f"u0 = chebfun(func, {ORDER_OF_INTERPOLATION}); "
            "run('heatfun.m'); "
        )
    subprocess.run(
        ["matlab", "-nodisplay", "-nosplash", "-nodesktop", "-r", f"{command}exit;"],
        cwd=BASE / "analysis",
    )


def matlab_solution(example_key: str):
    solution = np.loadtxt(BASE / "analysis" / f"matlab-result-{example_key}.csv", dtype=np.double)
    return solution


def analyse(example_key: str):
    u0 = heatfun.evaluateExpression(INTERESTING_U0[example_key], X_TO_PLOT)
    our_solution = heatfun_solution(example_key)
    chebfun_solution = matlab_solution(example_key)
    print(example_key, "squared error:", sum((our_solution - chebfun_solution) ** 2))

    fig = plt.figure()
    axes: matplotlib.axes.Axes = fig.add_subplot(2, 1, 1)
    axes.plot(X_TO_PLOT, u0, c=STYLE_COLORS[0]["color"], label="$u_0(x)$")
    axes.plot(X_TO_PLOT, chebfun_solution, c="k", label="$u_{cheb}(x, T)$")
    axes.plot(X_TO_PLOT, our_solution, "--", c=STYLE_COLORS[1]["color"], label="$u_{heat}(x, T)$")
    axes.set_ylabel("$u(x)$")
    axes.legend()
    axes: matplotlib.axes.Axes = fig.add_subplot(2, 1, 2)
    axes.semilogy(X_TO_PLOT, abs(our_solution - chebfun_solution), c=STYLE_COLORS[3]["color"])
    axes.set_xlabel("$x$")
    axes.set_ylabel("$|u_{heat} - u_{cheb}|$")
    tikzplotlib.save(
        RESULTS / f"comparison-{example_key}.tex",
        figure=fig,
        axis_width=r"0.8\linewidth",
        axis_height=r"0.25\linewidth",
    )
    fig.savefig(str(RESULTS / f"comparison-{example_key}.png"))


if __name__ == "__main__":
    # all_matlab_solutions()
    for key in INTERESTING_U0.keys():
        analyse(key)
    plt.show()
