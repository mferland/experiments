#!/bin/bash

apt update
apt -y upgrade
apt -y autoclean
apt -y autoremove

# install basic packages
apt -y install git
apt -y install mercurial
apt -y install subversion
apt -y install emacs24
apt -y install texlive

# TODO!!
