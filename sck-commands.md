Change XXX with the corresponding value.

### Basic SCK setup commands

* `$$$`							Wake uo the module and activates the wifi
* `set wlan ssid XXX\r`        Add a new SSID into memmory
* `set wlan phrase XXX\r`      Add a new phrase into memmory
* `set wlan key XXX\r`         Add a new Key into memmory
* `set wlan auth XXX\r`        Add a authentication method into memmory
* `set wlan ext_antenna XXX\r` Add an anthena type into memmory
* `exit\r`                     Goes back to normal operational mode


### Special SCK commands

* `get time update\r`          Retrieves sensors update interval
* `set time update XXX\r`    	Updates sensors update interval
* `get number updates\r`    	Retrieves the max number of bulck updates allowed
* `set number updates XXX\r`   Updates the max number of bulck updates allowed
* `get apikey\r`               Retrieves the kit APIKEY
* `set apikey XXX\r`           Updates the kit APIKEY
* `get wlan ssid\r`            Retrieves the SSID saved on the kit
* `get wlan phrase\r`          Retrieves the phrase and KEY saved on the kit
* `get wlan auth\r`            Retrieves the authentication methods saved on the kit
* `get wlan ext_antenna\r`     Retrieves the athena types saved on the kit
* `clear nets\r`               Removes all the saved WiFi configurations
* `exit\r`                     Goes back to normal operational mode
* `#data\r`  					Retrieves the sensor readings stored on the kit memmory