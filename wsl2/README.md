Install WSL2 Linux Kernel
=========================

This one is required for Windows docker users!

Below are the steps to install an alternative Linux kernel for Windows WSL2. Of course, you have to configure using WSL2 for your docker.

1.  If you plan to build a kernel by yourself, please follow steps 2~5 below. Alternatively, you may download our prebuilt kernel from [here](https://inp.zoolab.org/wsl/kernel.bzImage) and then skip to step 6.
    
2.  You can download the kernel source codes from Microsoft’s [github](https://github.com/microsoft/WSL2-Linux-Kernel).
    
3.  With the kernel source codes, you need to modify its configuration to enable queueing/scheduling features. The configuration file is located in [`Microsoft/config-wsl`](https://github.com/microsoft/WSL2-Linux-Kernel/blob/linux-msft-wsl-5.15.y/Microsoft/config-wsl). If you plan to use a text-based UI to modify the kernel configuration, you can run the command  
    `make KCONFIG_CONFIG=Microsoft/config-wsl menuconfig`  
    in the kernel source code directory.
    
    You have to enable all `QoS and fair queueing` (`Networking support` → `Networking options` → `QoS and fair queueing`) options in the kernel configuration. Please compile them as parts of the kernel instead of modules.
    
4.  You can build the kernel using the command  
    `make KCONFIG_CONFIG=Microsoft/config-wsl bzImage`.  
    You may want to append the `-j NN` parameter to accelerate kernel building with multiple CPU cores, where `NN` is the number of parallel building processes.
    
5.  If everything went smoothly, the kernel file would be placed in `./arch/x86/boot/bzImage`
    
6.  The official instructions for installing kernels using global options are available [here](https://learn.microsoft.com/en-us/windows/wsl/wsl-config). Here we summarize the steps below. We assume you place the kernel file in the `C:\wsl2\` directory, and the kernel is renamed to `kernel.bzImage`.
    
7.  Shutdown WSL2 using the command `wsl --shutdown`. If you have running docker instances, you should terminate your docker first.
    
8.  Create (or modify) the `%UserProfile%\.wslconfig` file. The `%UserProfile%` path is typically at `C:\Users\<username>`. If you are unsure, type the command `echo %UserProfile%` in a DOS command line prompt, or simply type `%UserProfile%` in the location bar of your File Explorer. A sample `.wslconfig` file is available [here](https://inp.zoolab.org/wsl/dot.wslconfig.txt). You can use it directly if you don’t have one.
    
9.  Restart your WSL2 system and docker, or reboot your computer. You can run the command `uname -a` in your WSL or Linux container to verify if you are running the new kernel. The reported Linux kernel version should be `5.15.133.1-microsoft-standard-WSL2` built on Nov 9, 2023.