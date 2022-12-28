import pathlib
import sys
import numpy as np

sys.path.append(str(pathlib.Path(__file__).parent.parent / "build" / "lib"))
import heatfun


def analyse():
    x = heatfun.modifiedChebpoints(30)
    u0 = np.exp(-(x**2) * 12)

    x_to_plot = np.linspace(-1.0, 1.0, 500)
    solution = heatfun.solve(u0, 0.003, x_to_plot)
    print(solution)


if __name__ == "__main__":
    analyse()
