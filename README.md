# RemoteLCD

## What is RemoteLCD?
**RemoteLCD** is a small project that allows you to communicate with a remote ESP32 device and display messages on a TFT LCD screen.

## What are the required components?
You will need:  
- an **ESP32-C3 Supermini**  
- a **TFT LCD** with the **ST7789 driver**, 1.9" 170x320 resolution

## Project structure
This project consists of two main parts:

- **The WebApp:** provides a simple password-protected interface that lets you write text to a file and access it later via an API token.  
- **RemoteLCD (ESP32):** contains the firmware source code for the ESP32. It uses **PlatformIO**, so you can open the `RemoteLCD` folder directly with it.

> The WebApp is optional — it’s mainly used to keep your text private instead of public.

## Setting up the WebApp
1. Copy the file `.env.example` to `.env`.  
2. Edit the `.env` file and fill in all the required environment variables.  
3. Build the project with:
   ```bash
   npm run build
