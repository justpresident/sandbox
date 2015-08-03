#!/bin/bash

function ping_mysql {
	HOST=$1
	while true; do
		DATE=`date +"%D %T"`
		RES=$( { date +"%D %T" && time { mysql -h $HOST -urplcat -pslvrkng yabsdb -e 'select 1' > /dev/null; }; } &>> /tmp/mysql_ping.$HOST.log )
		sleep 1
	done
}

CHILDS=""

for cluster in `seq -w 9 18`; do
	for dc in  e f h i; do
		HNAME="bscoll${cluster}${dc}"
		ping_mysql $HNAME &
		echo "ping $HNAME launched, PID=$!"
		CHILDS="$CHILDS $!"
	done
done

trap "{ kill $CHILDS ; exit; }" EXIT SIGINT SIGTERM

while true; do sleep 1; done

