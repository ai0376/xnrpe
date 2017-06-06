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

**command[check_disk_io]=../plugins/check_disk_io**

    this is xnrpe can run plugins exe. format is that: command[exe_name]=exe file location

this profile support # comment line


