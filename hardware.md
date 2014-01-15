Hardware
========

Components
----------

Now all the pieces that the SCK consists of will be described. Along each product a link will be attached which redirects to their datasheet.

Core components
---------------

### RTC Clock

The core board utilises a crystal oscillator that acts as a 16MHz real time clock. It needs an external power source to ensure it is always working and that is why you need to place a button cell in the bottom part of the core board.

### Atmel Mega 32U4-AU

As for the central processing unit the SCK uses an AVR chip from Atmel, as the Arduino Leonardo does. This is a low power, high performance 8-bit microcontroller, which at the same time has USB built-in capabilities, allowing operating systems to recognise it as a regular mouse.

### Energy modules

#### Solar panel

Any solar panel can be soldered to the board while it matches at least
our [minimum requirements][], that is: - 7V - 500mA

#### Battery

The battery we provide with the kit is powerful enough to power the SCK for a few consecutive days. However, if USB charging wants to be avoided a solar panel should be attached to the board.

Keep in mind that some tasks are more energy comsuming than others. For instance, writing information to the microSD card consumes less energy than directly uploading real-time information to the Internet.

Also, other parameters can be tuned by manually modifying the firmware in order to obtain more battery life. For instance, you can change the value of transmit global variable so communication with the remote database takes place less often.


### WiFly

This module enables a device to communicate using the standard IEEE 802.11 (Wi-Fi) b/g versions. It is Arduino friendly and the libraries can be found on [GitHub][], originally released by [Sparkfun][]. The libraries that the SCK uses are not the original ones. They are a lightweight modified version, specially built for the sake of this project.


### microSD card

When an Internet connection is not available, data is timestamped and saved in the microSD card, which allows to later upload all the gathered information.

Ambient shield components
-------------------------

This shield measures, as its name indicates, environmental factors. More precisely: temperature, humidity, CO, NO2, light and noise.

(To Do. Coming Soon...)
