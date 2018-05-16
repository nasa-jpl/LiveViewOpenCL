
String generic_help[] = {
    "SUMMARY\n",
    "camconfig provides camera configuration selections for your EDT PCI DV",
    "board and configures the PCI DV device driver based on your choice.",
    "\n\nUSAGE\n",
    "Make sure your camera is plugged in and powered on.",
    "\n\n",
    "Use the scroll bar to bring the choice that most closely matches the",
    "camera in use into view, then click on the choice. Press the \"OK\"",
    "button to commit your selection.",

    "\n\nMULTIPLE CAMERAS\n",
    "By default, camconfig configures a single PCIDV board (unit 0).",
    "If you have multiple PCI DVs, you can configure them all by using the '-n [nunits]'",
    "option. For example, if you have 2 pdvs, you should type",
    "\n\n",
    "camconfig -n 2",

    "\n\nSO WHAT DOES IT DO?\n",
    "camconfig writes the appropriate camera configuration commands to the file",
    "'pdvload', and then runs pdvload as a shell script. You can look at pdvload",
    "to see what it does. pdvload is run automatically at boot time, and can also be",
    "run any other time by simply by typing 'pdvload'. An example of when you might",
    "want to do this is if you booted the computer without having the camera turned",
    "on or plugged in (required for proper initialization of some cameras).",
    "\n\nNote that pdvload is a shell script that calls xchooser or achooser. These",
    "programs are designed to be invoked by camconfig, and should not be run directly.\n",
    NULL
};

