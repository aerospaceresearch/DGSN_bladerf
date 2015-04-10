# script for reading out IQ-Samples with a changing frequency using BladeRF
# This script was developed in context of space app challenge 2015 (Stuttgart)

import os,sys

# define Variables

freq_start 	= 300000000	# starting frequency
freq_end	= 310000000	# ending frequency
freq_step	= 2000000	# frequency shift after a record
bandwidth	= 2000000	# bandwidth to record
sps		= 2000000	# samples per second to record
time_step	= 2000		# time to scan each frequency in millisec

script_file = open("whoop.txt","w")
script_file.write("set bandwidth " + str(bandwidth) + "\n")

for freq in range(freq_start,freq_end+1,freq_step):
	script_file.write("set frequency " + str(freq) + "\n")
	script_file.write("rx config file=" + str(freq/1000000) + ".sc16q11 format=bin n=" + str(sps*time_step/1000)  + "\n")
	script_file.write("rx start\n")
	script_file.write("rx wait\n")

script_file.write("quit\n")
script_file.close()

os.system("bladeRF-cli -s whoop.txt")

# file to store all results, remove in later work!
output = open("result.sc16q11","wb")

for freq in range(freq_start,freq_end+1,freq_step):
	temp_file = str(freq/1000000) + ".sc16q11"
	while True:
		if os.path.isfile(temp_file):
			break
	print "File gefunden und wird gelesen"
	input = open(temp_file,"rb")
	output.write(input.read(-1))
	input.close()
	os.remove(temp_file)

output.close()
