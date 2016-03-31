#!/bin/bash

sudo apt update
exit 0
sudo apt -y upgrade
sudo apt -y autoclean
sudo apt -y autoremove

# install basic packages
sudo apt -y install git
sudo apt -y install mercurial
sudo apt -y install subversion
sudo apt -y install emacs24
sudo apt -y install texlive
sudo apt -y install i3
sudo apt -y install i3lock
sudo apt -y install valgrind
sudo apt -y install astyle
sudo apt -y install cppcheck
sudo apt -y install gitk

# install proggy font
mkdir -p $HOME/.fonts
cd $HOME/.fonts
wget http://www.proggyfonts.net/wp-content/download/ProggyClean.ttf.zip
unzip -o ProggyClean.ttf.zip
rm ProggyClean.ttf.zip
cd -

# emacs
mkdir -p $HOME/.emacs.d
install -m0644 dotemacs $HOME/.emacs.d/init.el
install -d $HOME/.emacs.d/auto-package
install -m0644 auto-package.el $HOME/.emacs.d/auto-package
cd $HOME/.emacs.d
rm -rf bb-mode
rm -rf nyan-mode
rm -rf helm
git clone https://github.com/TeMPOraL/nyan-mode.git
git clone https://github.com/mferland/bb-mode.git
git clone https://github.com/emacs-helm/helm.git
cd helm; make; cd ..
git clone https://github.com/jwiegley/emacs-async.git async
cd

exit 0

