# WoL-Remote-Broadcaster
A script for NodeMCU that accepts WoL packets, filters them by target MAC address and broadcasts them to the local network

## Why use this?
Most routers do not allow forwarding external WoL broadcast packets (192.168.0.255) from the WAN interface to the LAN for security reasons. This script turns a cheap NodeMCU into a secure proxy bridge that listens for your specific external request and wakes your PC from inside the network.

## Usage
With this setup you can safely enable your PC or any other WoL-compatible device using Wake-on-LAN from _anywhere_, as long as internet is accessible.

## Quick setup
1. Make sure you have a static (fixed) IP address 
2. Get NodeMCU
3. Download ArduinoIDE or any alternative software for programming microcontrollers
4. Modify settings in the script 
5. Upload the script to your NodeMCU
6. Forward any unoccupied port to your NodeMCU and bind it to static IP in your subnet (Use the control panel for your router. Check logs in ```debug = true``` mode to see the IP address)
7. Double-check the script settings so the ```localPort``` matched the port specified in router config, reupload if necessary
8. Send a WoL packet to your public IP with specified port
9. Profit
