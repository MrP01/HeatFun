import sys

import invoke


@invoke.task()
def build(ctx, mode="Debug"):
    ctx.run("mkdir -p build")
    with ctx.cd("build"):
        ctx.run("conan install ..")
        ctx.run(f"cmake -DCMAKE_BUILD_TYPE={mode} ..")
        ctx.run("make -j 4")


@invoke.task()
def analyse(ctx):
    ctx.run(f"{sys.executable} analysis/compare.py")
