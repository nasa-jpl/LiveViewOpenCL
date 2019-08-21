# Installation Guide for LiveView
## Quick Guide
Run these commands:
```
    $ git clone https://github.com/nasa-jpl/LiveViewOpenSource
    $ sudo apt-get install qt5-default     # Ubuntu/Debian
    $ brew install qt                      # MacOS
    $ cd LiveViewOpenSource/
** If installing with the CameraLink interface enabled, see below **
    $ qmake
    $ make -j
```
To install LiveView with the CameraLink interface available, first check [Installing the EDT PDV Driver](https://github.com/nasa-jpl/LiveViewOpenSource/wiki/Installation-Guide#Installing-the-EDT-PDV-Driver):
```
    $ sudo /opt/EDTpdv/setup.sh
    $ initcam -f /path/to/XXXX.cfg # camera config file
    $ take
```
Note that there is a test camera configuration file called NGIS.cfg in the source directory of LiveViewOpenSource.

## Detailed Guide
LiveView must be built from source, rather than installed as a pre-compiled executable because it is in active development. If there is sufficient demand for a particular platform, please open an [issue](https://github.com/nasa-jpl/LiveViewOpenSource/issues) mentioning the need for a pre-compiled binary. To begin, obtain a copy of the source from the GitHub by cloning the repository, entering this command in a terminal:

```
    $ git clone https://github.com/nasa-jpl/LiveViewOpenSource
```

If you wish to contribute to LiveView, and have not done so in the past, please configure your git profile at the terminal:

```
    $ git config --global user.name jryan
    $ git config --global user.email Jacqueline.Ryan@jpl.nasa.gov
```

Replacing "jryan" in the example above with your own username, and "Jacqueline.Ryan@jpl.nasa.gov" with your own email.

## Installing on Unix-based Operating Systems
### Getting Qt
[Qt](https://www.qt.io/) is a library for building desktop applications and is the main dependency of LiveView. To build the source, a copy of Qt 5.6 or newer is required. This software is free and can be installed via the one of the following methods:

* On Debian or Ubuntu: `sudo apt-get install qt5-default`
* On MacOS: `brew install qt`
* Alternatively, follow the [installation instructions on the Qt website](https://www.qt.io/download-qt-installer) 

### Getting QtCreator (Recommended, but Optional)
Once a copy of the source has been obtained, enter the source directory and open the LiveView.pro file in [QtCreator](https://doc.qt.io/qtcreator/). QtCreator is an IDE developed by the Qt Company, and is the editor used to develop LiveView. It is not required if the user does not plan to modify the code of LiveView.

```
    $ open LiveView.pro       # on MacOS
    $ qtcreator LiveView.pro  # on Linux
```

If QtCreator is not on the target machine, it can be installed using the package manager provided by the Linux distribution used on that machine. For instance, on Ubuntu and Debian, this would be:

```
    $ sudo apt-get install qtcreator
```

As an additional resource, Qt provides an [online installation document](https://wiki.qt.io/Install_Qt_5_on_Ubuntu) for Ubuntu. Likewise, On RedHat/CentOS, QtCreator can be installed with the following command:

```
    $ sudo yum install qtcreator
```

Additional documentation for installing Qt is also provided for [RedHat and CentOS](https://wiki.qt.io/How_to_Install_Qt_5_and_Qwt_on_CentOS_6). On MacOS:

```
    $ brew cask install qt-creator
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

## Installing OpenCL Drivers on Ubuntu
Some installations of Ubuntu will not have OpenCL installed by default. In that case, when attempting to compile LiveView, there will be compilation errors attempting to find the header "CL/cl.h". To install the OpenCL drivers, run the command:

```
    $ sudo apt install ocl-icd-* opencl-headers
```

## Configuring Camera Link drivers for LiveView
LiveView is compatible with Camera Link interfaces, which is a type of serial interface commonly used for connecting high-throughput devices to a computer. Engineering Design Team (EDT) provides an API for developing software to read from camera link devices using C++.

### Installing the EDT PDV driver
* See also: [Starting the EDT PDV driver](https://github.com/nasa-jpl/LiveViewOpenSource/wiki/Starting-the-EDT-PDV-driver)

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

The path to NGIS.cfg is the path to the top-level directory of the repository containing LiveView. If those commands succeed, then EDT is now configured to run on your machine. Each time the machine is rebooted, the `initcam` step will need to be re-run before starting LiveView. It is also a good idea to run take to ensure that the camera is working.

Sometimes, the camera config file is not the default. If the camera connected to the computer does not match the configuration file provided with LiveView, the image may "scroll" up or down across the screen, or the image may not look correct. Many more camera configuration files can be found here:
```
    $ cd /opgs/EDTpdv/camera_config
    $ initcam -f itb.cfg # for example
```

The specific configuration file will vary depending on the camera in use.

### Configuring LiveView to use a CameraLink interface
When starting up LiveView for the first time, a window will open asking the user to select which type of interface to use as a data input, either the SSD (in xio format), an ENVI format file, or a Camera Link device connected directly to the system.

If a user checks the box to "Never show this message again", but at a later point in time wishes to change their default input, the current method to change this election is to edit the configuration file. The `lvconfig.ini` configuration file will be located at `~/Library/Preferences/lvconfig.ini` on MacOS and `~/.config/lvconfig.ini` on Linux. Edit the value of the field "show_cam_dialog" from `false` to `true`.
