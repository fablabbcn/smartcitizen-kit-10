Change XXX with the corresponding value.

### Basic SCK setup commands

* `$$$`							           Wake up the module and activate the Wi-Fi
* `set wlan ssid XXX\r`        Add a new SSID to memory
* `set wlan phrase XXX\r`      Add a new phrase to memory
* `set wlan key XXX\r`         Add a new key to memmory
* `set wlan auth XXX\r`        Add an authentication method into memory
* `set wlan ext_antenna XXX\r` Add an antenna type into memmory
* `get mac\r`                  Get the MAC address of the kit
* `exit\r`                     Go back to normal operational mode


### Special SCK commands

* `get time update\r`          Retrieve the sensor update interval
* `set time update XXX\r`    	 Update the sensor update interval
* `get number updates\r`    	 Retrieve the max number of bulk updates allowed
* `set number updates XXX\r`   Update the max number of bulk updates allowed
* `get apikey\r`               Retrieve the kit APIKEY
* `set apikey XXX\r`           Update the kit APIKEY
* `get wlan ssid\r`            Retrieve the SSID saved on the kit
* `get wlan phrase\r`          Retrieve the phrase and KEY saved on the kit
* `get wlan auth\r`            Retrieve the authentication methods saved on the kit
* `get wlan ext_antenna\r`     Retrieve the antenna types saved on the kit
* `clear nets\r`               Remove all saved Wi-Fi configuration information
* `exit\r`                     Goes back to normal operational mode
* `#data\r`  					         Retrieves sensor readings stored in memory
