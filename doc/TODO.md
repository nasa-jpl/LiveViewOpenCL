# Items To Do for LiveView

## High priority items:
* Ensure that new data is added to the file buffer in SSDCamera when new images are being written to the directory
* Improve stability of frame display for CLCamera (Camera Link) at low backend framerates (<2 frames/second)
* Improve algorithm for computing fps to be time-based rather than frame count-based
* Related to the previous point, improve the reliablility of detection of no data an when the frame is out of sync (i.e., first line is in the middle of the frame
* Indicator when fps display is stable (?)


## Medium priority items:
* Finish porting features of original LiveView to new, experimental version
* Mean filter (profile is done)
* FFT filter
* Playback widget
* Preference window
* Save server and Python client
* Add a Reset button to the Standard Deviation tab and show when the display is stable (N frames acquired)
* Add a counter of the number of frames in the dark subtraction widget and allow a specific number of frames to be averaged for the dark mask.

## Low priority items:
* Show plot values on mouse hover in frameview tabs
* Allow tabs to be torn off the main window and put into new windows
* Allow dragging on the crosshair to change the range of the mean calculation.
* Provide compatibility for Yocto embedded Linux platforms
* Add DisplayPort capability
