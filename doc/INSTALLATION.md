# Installation Guide for LiveView
LiveView must be built from source, rather than installed as a pre-compiled executable because it is in active development. To begin, obtain a copy of the source from the JPL GitHub by cloning the repository, entering this command in a terminal:

```
    $ git clone https://github.jpl.nasa.gov/jryan/LiveView-Experimental
```

For git to work properly, ensure that you have logged into [JPL GitHub](https://github.jpl.nasa.gov/) at least once. If you wish to contribute to LiveView, and have not done so in the past, please configure your git profile at the terminal:

```
    $ git config --global user.name jryan
    $ git config --global user.email Jacqueline.Ryan@jpl.nasa.gov
```

Replacing "jryan" in the example above with your own username, and "Jacqueline.Ryan@jpl.nasa.gov" with your own email (preferably a JPL email address).

## Installing on Unix-based Operating Systems
Once a copy of the source has been obtained, enter the source directory and open the LiveView.pro file in QtCreator. If QtCreator is not on the target machine, it can be installed using the package manager provided by the Linux distribution used on that machine. For instance, on Ubuntu and Debian, this would be:

```
    $ sudo apt-get install qtcreator
```

As an additional resource, Qt provides an [online installation document](https://wiki.qt.io/Install_Qt_5_on_Ubuntu) for Ubuntu. Likewise, On RedHat/CentOS, QtCreator can be installed with the following command:

```
    $ sudo yum install qtcreator
```

Additional documentation for installing Qt is also provided for [RedHat and CentOS](https://wiki.qt.io/How_to_Install_Qt_5_and_Qwt_on_CentOS_6). On MacOS:

```
    $ brew install qt
```

Launch QtCreator, and open LiveView.pro within the new folder containing the program source. Select "Configure Project" in the window that opens, then wait for the source to be parsed and loaded in. Optionally, go to the Projects pane in QtCreator by selecting the wrench icon in the top left. In this view, under Build Steps, click the Details button for Make, the second of the two steps. Inside there, under "Make arguments:" enter "-j".

Next, At the bottom left corner of the window, click the Build button (Hammer). If there are errors, please report them to [Jackie](Jacqueline.Ryan@jpl.nasa.gov) as there may be an implementation-specific bug in the code. If the code compiles successfully, press the play button to run LiveView. A window should open, but if there is a crash, please report it. 

Once LiveView is running, minimize its window, and return to the QtCreator window. At the bottom of the window, select "3 Application Output". If there are a large number of errors regarding "[Errno XX] out of memory" or "[Errno NN] unable to lock memory" (TODO: Put the actual warning messages here), then there is a limitation in the amount of memory available to be page-locked by applications, which is constrained by the operating system.

This issue can be worked around by editing a file `/etc/security/limits.conf`. Close LiveView, and open the limits.conf file in a text editor with sudo permissions, for instance with vim:

```
    $ sudo vim /etc/security/limits.conf
```

There is a lot of explanation in the comments for this file, leave them alone and enter a new line at the bottom of this file:

```
rduce            hard    memlock         unlimited
```

Replace "rduce" with your own user name. For this change to affect the OS, log out of the machine and log back in. You can verify that amount of memory allowed to be locked by an application has been changed successfully by performing the following test in the comand line:

```
    $ ulimit -l # This will give the memory lock limit, which should be unlimited
    64
    $ ulimit -l unlimited # if the previous command does not return "unlimited", run this command
    $ ulimit -l           # ...and check again
    unlimited
```

If there is an error attempting to run the second command, `ulimit -l unlimited`, a reboot of the machine may be required.

Finally, re-open LiveView, either from QtCreator or from the new build directory that is located in the same top-level directory as the source. There should no longer be memory errors reported in the application console output. If so, then congratulations! LiveView has been successfully installed.

## Installing OpenCL Drivers for VM
For Intel CPU-only environments in VMs.  Drivers need to be installed from [https://software.intel.com/en-us/articles/opencl-drivers](https://software.intel.com/en-us/articles/opencl-drivers) instead of those availible on aptitute (for Ubuntu).

## Configuring Camera Link drivers for LiveView
LiveView is compatible with Camera Link interfaces, which is a type of serial interface commonly used for connecting high-throughput devices to a computer. Engineering Design Team (EDT) provides an API for developing software to read from camera link devices using C++.

### Installing the EDT PDV driver
To begin, go to the [EDT PDV downloads page](https://edt.com/file-category/pdv/). Select the Linux (.run) file option and download the script, you may have to right click and choose "Save Page As" if a plain text document opens up.

The .run file is a shell script, and can be executed in the terminal:

```
    $ chmod +x EDTpdv_lnx_5.5.5.4.run
    $ ./EDTpdv_lnx_5.5.5.4.run
```

Allow this script to complete, installing to the directory /opt/EDTpdv. Once installation is complete, go to /opt/EDTpdv and run the script `setup.sh`. Note that this script can fail cryptically if the operating system is not compatible with the version of PDV or the OS version has changed since the last time PDV was installed. PDV uses the linux system kernels to build a driver, so there can be compatibility issues. However, PDV has been tested and is known to work on Ubuntu 18.04.

Next, run the following two commands:

```
    $ initcam -f /path/to/NGIS.cfg
    $ take
```

The path to NGIS.cfg is the path to the top-level directory of the repository containing LiveView. If those commands succeed, then EDT is now configured to run on your machine. Each time the machine is rebooted, the `setup.sh` and `initcam` steps will need to be re-run.

### Configuring LiveView to use a CameraLink interface
Currently, since LiveView is in a development state, there is not a good, programmatic way that the camera interface can be switched while or before the program is running. As of right now, you will need to modify the code to use a Camera Link interface.

Eventually, LiveView will instead present a Dialog the first time it is opened which enables the user to select from a drop down menu which camera interface to use, and then save that preference in a configuration file so that the same interface will be used every time LiveView is opened from then on. Additionally, the GUI should provide the ability to change this preference at a later point in time (requiring a restart of LiveView for the changes to take effect).

To change the camera interface by modifying the code, open the file `frameworker.cpp` in QtCreator. The first line of the constructor should have a line like so:

```
    Camera = new SSDCamera();
```

which should be modified to:

```
    Camera = new CLCamera();
```

Rebuild and restart LiveView to verify that the program is now reading data from the Camera Link frame grabber card. If non-zero data appears in the window, then LiveView is now configured to use Camera Link.

## Contact
Please report any issues with installation to [Jackie Ryan](Jacqueline.Ryan@jpl.nasa.gov).

