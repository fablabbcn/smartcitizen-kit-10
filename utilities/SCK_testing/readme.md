Smartcitizen Kit testing script
=================


#### A python script to test and update Smartcitizen kits

##### It's a command line utility aimed to check the different parts of a kit and apply available updates in the most possible automated way.

* Checks wifly firmware version and installs available updates.
* Installs the latest available firmware from github (after first installation if you want to refresh the firmware, delete the folder named *"firmware"* before running the script again)
* Takes readings from sensors and checks if values are inside an acceptable range (*work in progress*).
* Creates a csv file with the results of the tests with one entry per tested kit.

------
##### Using the script

* This script requires a working installation of [python](https://www.python.org/) and [Arduino IDE.](https://www.arduino.cc/en/Main/Software) and a working Internet connection with wi-fi access.
* Download this folder to your computer, edit your network settings on line 9-10, connect your kit  to the USB port and run the script with `python SCKtester`.
* If you only have the kit connected to your computer and only one serial port is available, you can skip port selection with the *-a* option: `python SCKtester -p`