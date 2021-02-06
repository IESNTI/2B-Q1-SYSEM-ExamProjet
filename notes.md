## Mosfet avec ventilateur
- https://www.youtube.com/watch?v=Pw1kSS_FIKk
- https://www.onsemi.com/pub/Collateral/BS170-D.PDF
```
le - = fil noir
le + = fil jaune
```
## Récupérer température avec MPU9250
- https://github.com/bolderflight/MPU9250/blob/master/examples/Basic_I2C/Basic_I2C.ino
- http://arduinolearning.com/wp-content/uploads/2017/07/arduino-and-mpu9250_bb.png
- https://learn.sparkfun.com/tutorials/mpu-9250-hookup-guide/all#library-and-example-code

Faut faire bouger le module pour qu'il fonctionne
## Dialoguer avec ra-02
- https://create.arduino.cc/projecthub/334033/arduino-uno-lora-ra-02-temperature-sensor-d9ba95
- https://github.com/sandeepmistry/arduino-LoRa

## Communiquer avec le esp8266 / NodeMCU
Firmware: https://github.com/mlwmlw/esp8266-workshop/tree/master/firmware

Flasher le firmware sous linux (presque la même commande sous windows):
```
sudo esptool.py -p /dev/ttyUSB2 write_flash -fm dout 0x0000 v1.3.0.2\ AT\ Firmware.bin
```

Pour changer le baudrate:
```
AT+IPR=9600
```

Vidéo: https://www.youtube.com/watch?v=FMBzq8xdyww

Liens utiles:
- https://arduino.stackexchange.com/questions/18575/send-at-commands-to-esp8266-from-arduino-uno-via-a-softwareserial-port
- https://engineeringprojectshub.com/serial-communication-between-nodemcu-and-arduino/
- https://arduino.stackexchange.com/questions/74988/nodemcu-doesnt-response-in-at-command-mode
