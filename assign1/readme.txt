Authors
Obinna Elobi(100953254) - obinnaelobi@cmail.carleton.ca
Craig Isesele(100950074) - craigisesele@cmail.carleton.ca

Description
The assignment uses three programs cloud,controller and device. There is only one header file for all the programs

Bugs/Errors:
-Problem with timer: device process is slowed down so that items can be read/printed by processes as it is faster than them
-Cloud never stops running

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
