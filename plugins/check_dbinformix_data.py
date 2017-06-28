#!/usr/bin/evn python 
# -*- coding:utf-8 -*-
__author__ = 'mjrao'
__time__ = '2017/6/28'

'''
# yum install unixODBC
# install informix client sdk
# pip install pyodbc
# python >= 2.7
'''
import pyodbc
import json

def do_db_informix(dsn):

    #info_list=[]
    str = ''
    try:
        with pyodbc.connect('DSN=%s'%dsn) as conn:
            with conn.cursor() as  cursor:
                cursor.execute('select first 1 hostname,overtime from sip_st')
                rows = cursor.fetchall()
                for row in rows:
                    dd = dict(hostname=row[0],overtime=row[1])
                    str = '%s'%(json.dumps(dd))
    except Exception as e:
        #print e
        pass
    return str.decode('unicode_escape')

if __name__ == '__main__':
    print do_db_informix('informix')
    pass