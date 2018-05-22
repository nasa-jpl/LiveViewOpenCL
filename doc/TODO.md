# Items To Do for LiveView

## High priority items:
* Ensure that new data is added to the file buffer in SSDCamera when new images are being written to the directory
* Improve stability of frame display for CLCamera (Camera Link) at low backend framerates (<2 frames/second)
* Improve algorithm for computing fps to be time-based rather than frame count-based
* Indicator when fps display is stable/valid (issue: LiveView is currently showing the "real-time" results of the fps which take a time to stabilize)
* Improve the reliablility of detection of no image data and when the frame is out of sync (issue: LiveView currenly shows, some time, first line is in the middle of the frame)
* Install and Test Classic LiveView on CARBO GSE server Ubuntu computer (including standard deviation).
* Add installation step: "Edit /etc/security/limits.conf to include the line: `rduce        hard       memlock    unlimited`". In general, rduce should be replaced with the user of LiveView... Add a line to setrlimit in the code. A restart or re-login is required. This should be included in an install script (which could also build LiveView itself)


## Medium priority items:
* Finish porting features of original LiveView to new, experimental version
* Mean filter (profile is done)
* FFT filter
* Playback widget
* Preference window
* Save server and Python client
* Add a Reset button to the Standard Deviation tab and Histogram Tab and show when the display is stable (N frames acquired)
* Add a counter of the number of frames in the dark subtraction widget and allow a specific number of frames to be averaged for the dark mask.

## Low priority items:
* Show plot values on mouse hover in frameview tabs
* Allow tabs to be torn off the main window and put into new windows
* Allow dragging on the crosshair to change the range of the mean calculation.
* Provide compatibility for Yocto embedded Linux platforms with display capability (CARBO project flight Focal Plane Interface Electronics Digital platform: Xilinx UltraScale+ MPSoC Zynq)
* Add DisplayPort capability available on Yocto embedded Linux platforms ( CARBo project flight Focal Plane Interface Electronics Digital platform: Xilinx UltraScale+ MPSoC Zynq)
* Add connection to Alpha Data image server abstracting Alpha Data FPGA Hardware to allow multiple independent applications (LiveView, NGDCS, David Thompson CH4 Methane detection) to access the same data stream from computer DDR in place of multiple read SSD starving the limited data bandwdith of SSD
