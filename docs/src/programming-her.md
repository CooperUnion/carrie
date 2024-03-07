# Programming Her

Oh joy! `ssh` into the Pi for all of this.

## Packages You'll Need

This is what you'll need (probably):
```bash
apt update

# for docker (obvs)
apt install docker.io

# for pico sdk
apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

# for openocd
apt install libtool pkg-config autoconf automake texinfo
```

## Cloning the Repo

To clone the repo and initialize all the submodules:
```bash
git clone git@github.com:CooperUnion/carrie.git # if you have repo access
cd carrie/
git submodule update --init
```

## Creating & Using the Docker Image

There are some convenience scripts available for your convenience.
```bash
cd pi/
./container-init.sh # once, to create the container
./container-run.sh # whenever you need to use it
```

## Building OpenOCD
You'll have to use a custom-built version of OpenOCD, alas, to get things
working on the Pi. See [here](https://github.com/raspberrypi/openocd) for the
original instructions.
```bash
cd pico/openocd/
./bootstrap
./configure --enable-ftdi --enable-sysfsgpio --enable-bcm2835gpio
make
sudo make install
```

## Building the Firmware
There are more convenient convenience scripts for your convenience here, too!
```bash
cd pico/firmware/
./compile.sh
```

## Loading Code & Debugging

You'll want to be in a `tmux` session with a few panes for this.

In one pane, run the OpenOCD server. This allows for loading code & debugging
the Pico via SWD.
```bash
./openocd-server.sh
```

In another pane, start `gdb`:
```bash
./debug.sh
```

The `gdb` pane gives you full debugging power, live. To load new code onto the
Pico, compile the project, then run the following inside `gdb` (possibly after
pressing Ctrl-C to stop the Pico running):
```
(gdb) load
(gdb) monitor reset init
(gdb) continue
```

## Working with ROS

This could be (unfortunately) a book in itself. Basically, here's what you'll
need to do to start the ROS connection to the Pico, after getting into the container with `./container_run`:
```
colcon build # if you have new stuff you need to build
. install/local_setup.bash
ros2 run pico_bridge bridge --ros-args --log-level command_receiver:=DEBUG --log-level vicon_receiver:=DEBUG
```
This is not totally working right now, but that's the idea.
