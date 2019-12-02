#!/bin/sh
sudo apt-get install python-pip
sudo apt install dia
sudo apt-get install doxygen
sudo apt-get install graphviz
doxygen Doxyfile
xdg-open ../docs/html/index.html &