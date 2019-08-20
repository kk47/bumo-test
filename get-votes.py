#!/usr/bin/env python
# -- coding: utf-8 --

import os
import sys
import time
import sqlite3

currentdir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, currentdir + '/model')
from dpos_data import data_read, data_update
db_file = currentdir + '/dpos.db'

if __name__ == "__main__":
    data_update(db_file, url='http://seed1.bumo.io:16002/')
    votes=data_read('vote', db_file)
    voters = [v[1] for v in votes]
    amount = [v[4] for v in votes]
    
    votes_sum = 0
    for a in amount:
        votes_sum += a
    print "节点计划数据统计:\n截止到%s总共有%s人参与投票，投票总数：%swBU\n" % (time.strftime("%F %T"), len(set(voters)), votes_sum/1000000000000)
        
