#!/bin/bash
ulimit -n 4096

#判断进程是否存在，如果不存在就启动它
PIDS=`ps -ef |grep bottle_cap_detect |grep -v grep | awk '{print $2}'`
result=`ps -ef |grep bottle_cap_detect|grep -v grep`
if [ "$PIDS" != "" ]; then
echo "bottle_cap_detect is runing!" >> ./log.txt
echo "PIDS is $PIDS" >> ./log.txt

echo "result is $result" >> ./log.txt
else
./bottle_cap_detect >> ./log.txt
#运行进程
fi
