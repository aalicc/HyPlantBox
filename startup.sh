#!/bin/bash

cd /home/andreile/farmlab/innovationproj
lxterminal -e "onboard"
sleep 3
lxterminal -e "python3 /home/andreile/farmlab/innovationproj/cleanRuuvi.py"
sleep 5
lxterminal -e "python3 /home/andreile/farmlab/innovationproj/PythonScriptPROD.py"
sleep 1
lxterminal -e "chromium-browser"
sudo npm run dev