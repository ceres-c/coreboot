Up Squared - Microcode glitching target
===============

This is a modified coreboot version that enables custom microcode patches
as early as possible in the boot process to use this board as a target for
fault injection attacks.

Building
--------
Building this pretty much follows the standard coreboot build process, but you
probably want to see [here](https://ceres-c.it/coreboot/mainboard/up/squared/index.html#red-unlock)
how to enable the compile-time Red Unlock config.

Target code
-----------
You can select the target by changing the defines in `src/mainboard/up/squared/red_unlock.c`.
