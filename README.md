# 6.1120 Skeleton Code

This repository hosts the skeleton code for 6.1120, a class for building a virtual machine for a Python-like language called MITScript.

## Setup

### Docker

We recommend you use Docker for development. You'll first need to [install Docker Desktop](https://docs.docker.com/get-docker/).

To start a Docker container, run the following commands in this directory:

```sh
$ docker build -t mit-61120 .
$ docker run -it \
    --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
    --name=61120 --mount type=bind,source=${PWD},target=/src \
     mit-61120 bash
root@221b746bd366:/#
```

This will start the container, map the current repository in it, and
finally pull up a shell inside the container.
Once the shell starts, just write commands like you were using a regular
Linux terminal. The `/src` directory in the container is mapped to this directory on your machine.

```sh
root@221b746bd366:/# cd /src
root@221b746bd366:/src#
```

The Docker container stops automatically when your shell exits.
You can restart it from where you left off, with the command:

```sh
$ docker start -ai 61120
root@221b746bd366:/# cd /src
```

### Customization (Advanced)

If you want to customize your Docker image, edit the Dockerfile and build by running the command below.
Make sure you specify the same tag for the build and execution (`mit-61120` in the above).

```sh
# inside your project root
docker build . -t <image tag>
```

### Dependencies

If you're not using Docker, install the following dependencies on your local system:

```sh
sudo apt install git build-essential default-jre libantlr4-runtime-dev

# for P3
sudo apt install flex bison

# faster CMake backend for faster builds
sudo apt install ninja-build

# to help with debugging
sudo apt install valgrind
```

### Updating

To pull updates from this repository, go to your own repository, and run:

```sh
# only do this once
git remote add upstream git@github.com:mit-61120/61120-fa23.git

# do this every time the skeleton repository is updated
git pull upstream master
```

## Build and Run

### Build

The `build.sh` script will be used for building when grading,
so you should make sure it can build your code.
A `build.sh` is provided for you if you're planning to use CMake -
otherwise, you should modify it to fit your needs.

You can also use the script to build the code locally, or you can
run the commands directly as described below.

#### CMake

If you'd like to learn more about CMake, you can read the first
few chapters of the [tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html).
If you have more questions, ask on Piazza!

To avoid starting every build from scratch, you can run the CMake build commands directly.

```sh
# This is called the configuration step and it prepares the build system inside cmake-build-grading.
# Rerun only if you deleted the build tree (the cmake-build-* directory), or if you want to change CMake options (-D etc.)
cmake -D CMAKE_BUILD_TYPE=Release -S . -B cmake-build-grading

# Run this in all other scenarios. It will rerun the configuration step, and then perform the incremental build
cmake --build cmake-build-grading

# clean compiled files
cmake --build cmake-build-grading --target clean

# only build a specific target (e.g. interpreter):
cmake --build cmake-build-grading --target mitscript

# clean and build
cmake --build cmake-build-grading --target mitscript --clean-first
```

This will put the executables inside `cmake-build-grading/interpreter/*`.
You can use the symlinks that `build.sh` creates, but you don't have to.

##### Further Customization

CMake has a lot of customization options. E.g. to compile without optimization
and with debugging symbols, do the following:

```sh
# we can keep multiple builds around with different options by
# storing them in separate directories.
cmake -D CMAKE_BUILD_TYPE=Debug -S . -B cmake-build-debug
cmake --build cmake-build-debug
```

If you want to add compile options or flags, use:

- [`target_compile_options`](https://cmake.org/cmake/help/latest/command/target_compile_options.html)
- [`target_compile_definitions`](https://cmake.org/cmake/help/latest/command/target_compile_definitions.html)
- etc.

Make sure you're looking at documentation for your version of CMake (`cmake --version`).

If you want to run with the Ninja backend for better compilation speed,
specify it with the `-G` option during the configuration step;

```sh
cmake -D CMAKE_BUILD_TYPE=Debug -G Ninja -S . -B cmake-build-debug

# build step stays the same, just make sure Ninja is installed.
```

##### Building Tests

By default, tests won't build.
To build them, add `-D BUILD_TESTS=ON` (before the `-S` flag) to the configure step:

```sh
cmake -D CMAKE_BUILD_TYPE=Debug -D BUILD_TESTS=ON -S . -B cmake-build-debug
```

#### Make

Uncomment line 2 of `build.sh`, and you're all set. To run directly,
just run `make` inside the `interpreter` directory.

Unit tests are not currently supported when using make.

#### IDEs (CLion, Visual Studio Code, etc.)

These will usually work with CMake right out of the box.
Ask on Piazza if you have trouble connecting your IDE to the build system.

### Run

Whether your parser is inside `interpreter/` or `cmake-build-*/interpreter`, invoke it as such:

```sh
[path/to/executable]/mitscript-parser [path/to/input/file]
```

#### Running Tests

To run the provided tests, please see the `tests/[phase]/run-tests.sh` scripts. The top of each script contains instructions for running the tests.

#### Unit tests

To run parser unit tests:

- Create a test file (like `test/interpreter/example-test.cpp`)
- Add the test file to the correct target in `CMakeLists.txt`
- Build the tests (described above)
- Run the tests:

```sh
cmake-build-debug/test/interpreter/parser-tests [flags]
```
