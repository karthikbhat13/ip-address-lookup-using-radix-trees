#!/bin/bash

#
#
# got it from https://moz.com/top500
#

file='top500domains.txt'
#
IFS=","

while read rank url rootd ext mrank trust
do
	#
	# Remove double quote
	#
	url=${url//\"}
	#
	# Remove forward /
	url=${url//\/}
	
	#
	# prefix www to the ip address
	#
	url="www."${url}
	ipaddr=`nslookup $url 2> /dev/null | grep -v Non | grep "Address" | tail -1 | awk '{print $2}'` 
	printf "%s, %s\n" $url $ipaddr
		
	
done < $file
