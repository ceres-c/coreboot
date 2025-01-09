#!/bin/sh
#
# This is a trivial payload compile & find script for abuild
#
DIR=$(dirname "$0")
lpgcc -o "$DIR/ucodeupdate.elf" "$DIR/ucodeupdate.c" >/dev/null 2>&1 || exit 1
echo "$DIR/ucodeupdate.elf"
