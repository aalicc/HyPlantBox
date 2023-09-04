#!/bin/bash

cd /home/andreile/farmlab/innovationproj
lxterminal -e "onboard"
lxterminal -e "chromium-browser"
lxterminal -e "python3 /home/andreile/farmlab/innovationproj/cleanRuuvi.py"
lxterminal -e "python3 /home/andreile/farmlab/innovationproj/PythonScriptPROD.py"
sudo npm run dev