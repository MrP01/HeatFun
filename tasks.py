import invoke


@invoke.task()
def build(ctx, mode="Debug"):
    ctx.run("mkdir -p build")
    with ctx.cd("build"):
        ctx.run("conan install ..")
        ctx.run(f"cmake -DCMAKE_BUILD_TYPE={mode} ..")
        ctx.run("make -j 4")
