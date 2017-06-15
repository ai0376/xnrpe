# XNRPE README

## Contents

1 **src/xnrpe**-this program runs report linux system infos

2 **plugins/**-those are some plugins that can get system infos

3 **test_server/sample_server** -this is a sample server to test xnrpe

## Compiling

* compiling **plugins**
```
$cd plugins
$make
```
* compliling **xnrpe**
```
$cd src
$make
```
* compliling **sample_server**
```
$cd test_server
$make
```

## Run Test

* run sample_server
```
$./sample_server 65510
```

* run xnrpe
```
$./xnrpe -c xnrpe.conf
```

## Profile description

### xnrpe.conf

**server_address=127.0.0.1**
    
    reporting destination server address
**server_port=65510**

    reporting destination server port 
**localhost=192.168.2.21**
    
    reporting source address 
**sock_send_recv_timeout=5**

    socket send and recv timeout(seconds)
**heartbeat_time=120**
    
    heartbeat time(seconds)
**report _time=10**

    report time(seconds)
**daemonize=yes**

    By default xnrpe does not run as a daemon. Use 'yes' if you need it

    Note that xnrpe will write a pid file in /var/run/xnrpe.pid when daemonized
**logfile=/var/log/xnrpe.log**

    Specify the log file name. Also the empty string can be used to force
    xnrpe to log on the standard output. Note that if you use standard
    output for logging but daemonize, logs will be sent to /dev/null

**pidfile=/var/run/xnrpe.pid**

    Creating a pid file is best effort: if xnrpe is not able to create 
    it nothing bad happens, the server will start and run normally.

**command[check_disk_io]=../plugins/check_disk_io**

    this is xnrpe can run plugins exe. format is that: command[exe_name]=exe file location

this profile support # comment line


