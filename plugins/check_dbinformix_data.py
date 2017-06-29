#!/usr/bin/evn python 
# -*- coding:utf-8 -*-
__author__ = 'mjrao'
__time__ = '2017/6/28'

'''
# yum install unixODBC
# install informix client sdk;  ./installclient -gui
# pip install pyodbc
# python >= 2.7
'''
import pyodbc
import json
import sys

neTopType ='PF_DBINFORMIX_DATA';

def do_db_informix(dsn):

    #info_list=[]
    str = ''
    i = 0
    try:
        with pyodbc.connect('DSN=%s'%dsn) as conn:
            with conn.cursor() as  cursor:
                cursor.execute('select first 1 hostname,overtime from sip_st')
                rows = cursor.fetchall()
                for row in rows:
                    dd = dict(hostname=row[0],overtime=row[1])
                    if i == 0:
                        return dd
                    else:
                        pass
    except Exception as e:
        pass
    #return str.decode('unicode_escape')

if __name__ == '__main__':
    
    if len(sys.argv) != 2:
        pass
    else:
        id = sys.argv[1]
        info = do_db_informix('informix')
        if info:
            dic = dict(values=info,neTopType=neTopType,neId=id,neName='')
            #print json.dumps(dic).decode('unicode_escape')
            print json.dumps(dic)
    pass