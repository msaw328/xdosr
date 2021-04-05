xdosr - Experimentally Developed Operating System (Revisited)

This repository contains an attempt at writing a kernel for x86 platform (64 bit) as well as a UEFI bootloader for loading said kernel. Despite the name it is not related to DOS family of operating systems in any way.

It contains two directories, one for kernel and the other for the bootloader, which both try to conform to the Pitchfork project layout specification (https://github.com/vector-of-bool/pitchfork).

The EDK2 header files are used to interact with the UEFI subsystem (https://github.com/tianocore/edk2), though the library itself is not linked.
