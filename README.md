# Up Squared - Microcode glitching target
This is a modified coreboot version that enables custom microcode patches
as early as possible in the boot process to use this board as a target for
fault injection attacks.

This repo is part of the [MicroSpark](https://github.com/ceres-c/MicroSpark) project.

## Building
Building this pretty much follows the standard coreboot build process, but you
probably want to see [here](https://ceres-c.it/coreboot/mainboard/up/squared/index.html#red-unlock)
how to enable the compile-time Red Unlock config.

This repo also contains an IFWI file with the red unlock exploit developed by
the [lib-micro](https://github.com/zanderdk/lib-micro) authors (the one on
their website is non-functional tough).

## Target code
You can select the target by changing the defines in `src/mainboard/up/squared/red_unlock.c`.
