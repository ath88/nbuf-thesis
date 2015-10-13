sudo echo 'Clearing system RAM file cache.'
sync ; sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
