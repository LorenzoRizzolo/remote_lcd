# RemoteLCD

## What is RemoteLCD?
**RemoteLCD** is a small project that allows you to communicate with a remote ESP32 device and display messages on a TFT LCD screen.

## What are the required components?
You will need:  
- an **ESP32-C3 Supermini** or **ESP32 classic** 
- a **TFT LCD** with the **ST7789 driver**, 1.9" 170x320 resolution in my case, but you can personalize

## Setting up the Esp32 or Esp32-c3 supermini
in the folder `ESP32_RemoteLCD` you have a file `ESP32_RemoteLCD\config.h.example` to copy into `ESP32_RemoteLCD\src\config.h` and `ESP32_RemoteLCD\config.cpp.example` to copy into `ESP32_RemoteLCD\src\config.cpp` where you need insert your credentials for wifi connection and other params.

For flash and open serial monitor for esp32c3 use
```bash
sudo pio run -e esp32-c3 -t upload && sudo pio device monitor -e esp32-c3 -b 115200
```
and for esp32 classic
```bash
sudo pio run -e esp32 -t upload && sudo pio device monitor -e esp32 -b 115200
```

- **-e** is the environment, **esp32c3** or *esp32*
- **-t** is the target, beacouse we want upload the code to the device
- **-b** is the baud rate of the serial communication, the speed