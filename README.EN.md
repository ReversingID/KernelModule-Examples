# Kernel Module Examples

Simple examples for learning Kernel Module of various platform.

### Overview

Untuk versi Bahasa Indonesia, lihat [README.md](README.md).

This repository collect some sample codes to build Kernel Module of various platform.

All code listed in this repository provide instruction for compilation.

### Introduction to Kernel Module

Kernel Module is a code snippet which extends or enrich the functionality of kernel of operating system. Most of operating system support Kernel Model using its own term. Some popular term are:

- `Kernel Loadable` module (kld) for `FreeBSD`
- `Kernel Extension` (kext) for `MacOS`
- `Loadble Kernel Module` (lkm) for `linux`

Kernel Module is often referred as driver or `device driver`. Although some kernel modules are created to implement interface between kernel and devices, this is not the full capability of a kernel module. Kernel module is not only used as interface to device, but to implements certain feature in kernel.

Support for kernel module makes kernel modular and more flexible. Extending the funcitonality of kernel can be done without rebuilding kernel everytime changes happen.

### Disclaimer

This repository is an initiative of Reversing.ID to learn Kernel Module development better.

### How to Download?

This repository can be downloaded as whole (clone) or some code can be downloaded individually.

### How to Contribute?

This is open project.

You can contribute by submitting code for any platform, adding or modifying existing code to give more information.

To do that, you can either:

- do pull request.
- send email to pengurus [at] reversing.id
- contact us at telegram @ReversingID