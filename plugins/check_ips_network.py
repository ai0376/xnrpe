#!/usr/bin/evn python 
# -*- coding:utf-8 -*-
__author__ = 'mjrao'
__time__ = '2017/6/26'

import telnetlib
import sys
import json

#cmd = ':display-mtp-network:netid=all;'
cmd = 'ls'

def do_telnet(host, port, user, password, suc):
    tn = telnetlib.Telnet(host=host, port=port)
    tn.read_until('login: ')
    tn.write(user + '\n')

    tn.read_until('Password: ')
    tn.write(password + '\n')
    tn.read_until(suc)
    tn.write(cmd + '\n')

    outbuf = tn.read_until(suc)
    tn.close()
    del tn
    #print outbuf
    dlist = []
    list_obj = outbuf.split('\n')
    lsize = len(list_obj)
    if list_obj[0].find(cmd) !=  -1:
        dlist = list_obj[1:lsize-1]
    else:
        dlist = list_obj[0:lsize-1]
    
    ss = dlist[0].strip('\r')
    array = ss.split()
    return array

#
#$python check_ips_network.py host port user password suc
#
if __name__ == '__main__':

    argc = len(sys.argv)
    if argc != 6:
        exit()
    host = sys.argv[1]
    port = int(sys.argv[2])
    user = sys.argv[3]
    password = sys.argv[4]
    suc = sys.argv[5]

    print do_telnet(host, port, user, password, suc)
    