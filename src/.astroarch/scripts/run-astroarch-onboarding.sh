#!/bin/sh

# Link folders to root folder
rm -f /root/.oh-my-zsh
ln -s /home/astronaut/.oh-my-zsh /root
rm -f /root/.astroarch
ln -s /home/astronaut/.astroarch /root
rm -f /root/.zshrc
ln -s /home/astronaut/.astroarch/configs/.zshrc /root/.zshrc
chsh -s /usr/bin/zsh root

# Allow root access with no password to astroarch-onboarding.desktop
rm -f /usr/share/polkit-1/actions/com.github.calamares.calamares.policy
sudo ln -s /home/astronaut/.astroarch/configs/com.github.calamares.calamares.policy /usr/share/polkit-1/actions/com.github.calamares.calamares.policy
sudo ln -s /home/astronaut/.astroarch/configs/49-nopasswd-calamares-vnc.rules /etc/polkit-1/rules.d/49-nopasswd-calamares-vnc.rules
sudo ln -s /home/astronaut/.astroarch/configs/49-nopasswd-calamares-xrdp.rules /etc/polkit-1/rules.d/49-nopasswd-calamares-xrdp.rules
