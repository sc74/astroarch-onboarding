#!/bin/bash

sed -i 's/%wheel ALL=(ALL:ALL) ALL/%wheel ALL=(ALL:ALL) NOPASSWD: ALL/g' /etc/sudoers

su astronaut -c "zsh -c 'source /home/astronaut/.zshrc && update-astroarch'"

sed -i '0,/%wheel ALL=(ALL:ALL) NOPASSWD: ALL/s/%wheel ALL=(ALL:ALL) NOPASSWD: ALL/%wheel ALL=(ALL:ALL) ALL/' /etc/sudoers
