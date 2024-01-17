Install pwntools
================

`pwntools` is a CTF framework and exploit development library written in Python. It might not be mandatory for solving lab challenges, but it makes your life easier. It is a python package that allows you to interact with a remote server (or a local process) _**programmatically**_. It is typically used for solving CTF challenges. You can find its document [here](https://docs.pwntools.com/en/stable/). To install `pwntools` in your environment, please select the appropriate methods listed below that matches your runtime. We use `virtualenv` package to ensure the installed packages are not mixed up with others. We assume the `pwntools` will be installed in the `~/pwntools` directory. You may choose your preferred installation directory.

*   If you run our _**Debian Linux**_ docker, you can install it using the commands:
    
        apt-get install -y python3-pwntools
        
    
    If you run our docker on M1/M2, you may need to install additional `binutils` package.
    
        apt-get install -y binutils-x86-64-linux-gnu
        
    
*   If you run our _**Ubuntu Linux**_ docker on an _**x86\_64**_ machine, we recommend you to install it in `virtualenv` using the commands:
    
        virtualenv -p python3 ~/pwntools
        . ~/pwntools/bin/activate
        pip3 install --upgrade pwntools
        
    
*   If you run our _**Ubuntu Linux**_ docker on an _**arm64 (e.g., Apple M1/M2)**_ machine, you can install it using the commands:
    
        virtualenv -p python3 ~/pwntools
        . ~/pwntools/bin/activate
        pip3 install --upgrade unicorn==1.0.3 pwntools
        
    
*   If you plan to run it natively on your mac (both Intel and Apple chips), you can install it using the [`homebrew`](https://brew.sh/) package manager.
    
        brew install pwntools binutils x86_64-elf-binutils
        
    
    Note that `homebrew` may be installed in `/opt/homebrew` or in `/usr/local`. Once `pwntools` is installed, you can activate its virtualenv installed by `brew` using the command (__**assume pwntools 4.8.0 is installed**__) you have to use the python binary installed in the `pwntools` directory.
    
        . /usr/local/Cellar/pwntools/4.8.0/libexec/bin/activate
        
    
    or
    
        . /opt/homebrew/Cellar/pwntools/4.8.0/libexec/bin/activate
        
    
    depending on the installation directory. If your virtual env does not has the `activate` script, you may re-generate it using the command
    
        virtualenv </path/to/your/pwntools/virtualenv/root>
        
    
    For example,
    
        virtualenv /opt/homebrew/Cellar/pwntools/4.8.0/libexec
        
    

To validate your installation, ensure that your `pwntools` has been appropriately installed and activated, and type the command in `python3` interpreter: `from pwn import *`. Your installation is successful if you do not receive any error messages.