Authors
Obinna Elobi(100953254) - obinnaelobi@cmail.carleton.ca
Craig Isesele(100950074) - craigisesele@cmail.carleton.ca

Description
The assignment uses three programs cloud,controller and device and one header file.

The device sends randomly generated ints to represent real world random data and transmits it to the controller via a message queue. the controller processes the data and if its above a threshold it sends a signal to the device and transmits the data via a named pipe to the cloud



Instructions for running code:
-Open 3 command line terminals
-Firstly type in 'make' in any one of 
-Then type "./cloud" in the last terminal  
-Type "./controller" in a separate terminal
-Then type "./device devicename1 threshold1 & ./device devicename2 threshold2" in another terminal
-Make sure to press enter only after you've typed in all the values

Miscellaneous
-The inputted threshold should not have a value above 1000
-To stop the program type in Ctrl+C in the terminal containing controller
