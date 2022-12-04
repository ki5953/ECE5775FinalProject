# Iris Flower Classification Using Gaussian Naive Bayes

## Run on ecelinux
* ```cd ECE5775FinalProject/ecelinux```
* ```make float_vivado```

## Generate bitstream
* ```cd ECE5775FinalProject/ecelinux```
* ```make float_bitstream```
* Secure copy ```xillydemo.bit``` to Zedboard
* Secure copy ECE5775FinalProject.zip to Zedboard
* Log in to Zedboard, mount SD card, copy the bitstream to the SD card and reboot the Zedboard

## Run on ARM Zedboard and FPGA
* Log in to Zedboard
* Unzip ECEFinalProject.zip
* ```cd ECE5775FinalProject/zedboard```
* ```make sw``` -- running on ARM Zedboard
* ```make fpga``` -- running on FPGA
