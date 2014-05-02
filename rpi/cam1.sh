sudo raspivid -n -o - -t 9999999 -w 1024 -h 768 -b 1000000 -fps 2|nc  -l 8081
