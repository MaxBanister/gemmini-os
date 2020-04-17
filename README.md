# GemminiOS / Firmware

**Author:** Max Banister
**Email:** maxbanister@berkeley.edu

Companion software to [Gemmini Device Driver](https://github.com/MaxBanister/gemmini-device-driver)

## Overview

Gemmini is enabled through an extension to the RISC-V ISA that only runs on a singular hart. Specialized firmware, GemminiOS, is run on this hart for the sole purpose of running Gemmini-accelerable kernels. This approach comes with the advantages that the software is simple and there is practically no scheduling overheard to running a kernel, as all the scheduling happens in the device driver on the Linux side. However, the drawback is that one general purpose Rocket core sits idle most of the time, which decreases system throughput, but also reduces power consumption. There is also more latency to be expected with this approach than if all cores uniformly supported Gemmini's ISA extension, but insomuch as it is impractical to have every core support the Gemmini ISA, this is necessary.

## Build Instructions

The firmware is essentially a fork of Berkeley Bootloader, as the mechanisms for inter-processor communication had to be modified to support communication between the Gemmini hart and the Linux harts. The primary change was to add a subproject `gemmini` that describes the S-mode "operating system" that receives interrupts and runs function pointers passed from Linux. The other, smaller changes, were to the `machine` subproject to support Gemmini-specific messages that needed to be passed between the two harts. This guide will specify how to build this fork of bbl, but if you want to apply only patches on top of a more up-to-date bbl, please see the "Patching BBL" subsection below.

1. First, clone the repository into a scratch directory, and follow the guide to building Linux in https://github.com/MaxBanister/gemmini-device-driver if you haven't already.

2. Run the configuration script:

```
autoconf
cd build
../configure --prefix=$RISCV --host=riscv64-unknown-linux-gnu --with-payload=/PATH_TO_LINUX/vmlinux --with-payload1=bbl
make
```

And optionally, to put this version of bbl on your path:
```
make install
```

3. And voilà! You should have a working version of bbl built with two separate binaries: Linux and GemminiOS. To run, simply type:

```
spike -p2 bbl
```

The `-p2` spawns two cores on which to run our modified version of bbl, which is necessary for both Linux and GemminiOS to run. `-pX` where X > 2 is also supported.

## Patching BBL

If you would prefer to start with a clean clone of `riscv-pk`, perhaps because because it contains changes not yet integrated into this fork, here are the instructions for applying the changes to bbl on top of the fresh version.

Navigate to the top-level directory of the `riscv-pk` clone that you wish to apply the patches to, and run:

```
patch < riscv-pk-gemmini.patch
```

Now, follow the make steps in the above section.

RISC-V Proxy Kernel and Boot Loader
=====================================

About
---------

The RISC-V Proxy Kernel, `pk`, is a lightweight application execution
environment that can host statically-linked RISC-V ELF binaries.  It is
designed to support tethered RISC-V implementations with limited I/O
capability and thus handles I/O-related system calls by proxying them to
a host computer.

This package also contains the Berkeley Boot Loader, `bbl`, which is a
supervisor execution environment for tethered RISC-V systems.  It is
designed to host the RISC-V Linux port.

Build Steps
---------------

We assume that the RISCV environment variable is set to the RISC-V tools
install path, and that the riscv-gnu-toolchain package is installed.
Please note that building the binaries directly inside the source
directory is not supported; you need to use a separate build directory.

    $ mkdir build
    $ cd build
    $ ../configure --prefix=$RISCV --host=riscv64-unknown-elf
    $ make
    $ make install

Alternatively, the GNU/Linux toolchain may be used to build this package,
by setting `--host=riscv64-unknown-linux-gnu`.

By default, 64-bit (RV64) versions of `pk` and `bbl` are built.  To
built 32-bit (RV32) versions, supply a `--with-arch=rv32i` flag to the
configure command.

The `install` step installs 64-bit build products into a directory
matching your host (e.g. `$RISCV/riscv64-unknown-elf`). 32-bit versions 
are installed into a directory matching a 32-bit version of your host (e.g.
`$RISCV/riscv32-unknown-elf`).

OpenBSD Build Steps
-------------------

Install the riscv-gnu-toolchain, and follow generic build steps.

    # pkg_add riscv-elf-binutils riscv-elf-gcc riscv-elf-newlib

   
