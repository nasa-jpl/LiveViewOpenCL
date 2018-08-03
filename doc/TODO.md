# Items To Do for LiveView

## High priority items for Jacky:
*	Fix “override file” dialog box that overrides no matter what is selected -  
* Install and Test standard deviation LiveView on CARBO GSE server Ubuntu computer

## High priority items for Brandon (10 weeks) using CARB-O Alpha Data UltraScale+ MPSoC board:
*	Sync of first line when there is a drop, no more need to relaunch liveview – While I did implement this functionality, it unfortunately did not play well with the frame counter for some reason, causing every other frame to be dropped (or perhaps not counted). Additionally, since the fps counter in this version of LiveView is count-based, slower framerates take a long time to update the value in the GUI, and so at 1 fps (or a timeout condition), the fps value in the GUI is only updated every 100 secs (!!). I will need to re-visit this in further detail, it may be an issue with the EDT API.
* Provide compatibility for Yocto embedded Linux platforms with display capability (CARBO project flight Focal Plane Interface Electronics Digital platform: Xilinx UltraScale+ MPSoC Zynq)
* Add DisplayPort capability available on Yocto embedded Linux platforms ( CARBo project flight Focal Plane Interface Electronics Digital platform: Xilinx UltraScale+ MPSoC Zynq)
* Improve stability of frame display for CLCamera (Camera Link) at low backend framerates (<2 frames/second)
* Improve the reliablility of detection of no image data and when the frame is out of sync (issue: LiveView currenly shows, some time, first line is in the middle of the frame)

## Medium priority items:
* Finish porting features of original LiveView to new, experimental version
* Implement FFT filter plots and visualizations of the legacy liveview
* Implement Playback widget plots and visualizations of the legacy liveview
* Preference window
* Save server and Python client
* Add a counter of the number of frames in the dark subtraction widget and allow a specific number of frames to be averaged for the dark mask.

## Low priority items:
*	Many of our users ask if we can interpolate between pixels, for example, fitting an optical signal to a Gaussian curve. It would be useful for alignment (and as you know, alignment is a big deal…) – Not yet implemented. I’ve heard of this idea, but I want to make LiveView 4 a full replacement of the old LiveView first.
* Show plot values on mouse hover in frameview tabs
* Allow tabs to be torn off the main window and put into new windows
* Allow dragging on the crosshair to change the range of the mean calculation.
* Add connection to Alpha Data image server abstracting Alpha Data FPGA Hardware to allow multiple independent applications (LiveView, NGDCS, David Thompson CH4 Methane detection) to access the same data stream from computer DDR in place of multiple read SSD starving the limited data bandwdith of SSD
*	Stream down-sampled version of data to iPad app (hey it’s an idea…) using iOS development. It could be similar to the Frame Server concept implemented by NGDCS

## General Implementation Policy:
*	Keep with using QT and standard linux libraries for the core buildup

## DONE:
* Drop CUDA and EDT as build requirements: this is complete. EDT is an optional dependency in the new LiveView, which will eventually be able to be toggled in a config file.
*	implement universal camera interface (including recorded files). Make it flexible to support a variety of hardware i/o: This is implemented for EDT Camera Link interfaces and files on the SSD (in particular, files in the format used in Didier’s lab)
*	Use OpenCL instead of CUDA where we need GPU power: OpenCL has been implemented, and I can confirm that the Standard Deviation is performed by the GPU when available and can optionally be run on the CPU
*	Implement all plots and visualizations of the legacy liveview: All visualizations are complete with the exception of the FFT Display and the Playback Widget.
* Install and Test Classic LiveView on CARBO GSE server Ubuntu computer
* Improve algorithm for computing fps to be time-based rather than frame count-based
* Indicator when fps display is stable/valid (issue: LiveView is currently showing the "real-time" results of the fps which take a time to stabilize)
* Mean filter (profile is done)
* Enhance remote control TCP/IP interface. This has been done in theory, although many features still need to be built out, such as requesting a default save location, getting the status of the frame save operation, etc.
