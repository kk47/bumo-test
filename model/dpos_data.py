#!/usr/bin/env python
# -*- coding: utf-8 -*-

# dpos_data_view - view dpos contract data
# Author: kuangkai@bubi.cn
# Date: 2019-4-3 10:31

import sqlite3 as lite
import sys
import json
import time
import logging
from dpos_test import ChainApi 

metadatas_hash = 'xxx'

tables = ['validator_candidates', 'kol_candidates', 'committee', 'proposal', 'vote', 'validator_reward_distribution', 'kol_reward_distribution', 'configuration']

def data_get(url):
    ''' get all dpos data from contract account '''

    global metadatas_hash
    payload = 'address=buQqzdS9YSnokDjvzg4YaNatcFQfkgXqk6ss'
    ca = ChainApi(url)
    res = ca.req('getAccount', payload)
    if not res or res['error_code'] != 0:
	return False, 'Failed to get dpos data'
    elif res['result']['metadatas_hash'] == metadatas_hash:
	return False, 'No metadata change'
    metadatas_hash = res['result']['metadatas_hash']
    return True, res

def data_parse(cur, data):
    ''' Parse data from contract metadata '''
	
    proposals = []
    votes = []
    cfg = {}
    validator_reward_dist = {}
    kol_reward_dist = {}
    validator_cands = []
    committee = []
    kol_cands = []
    
    for kv in data:
	key = kv['key']
	if key.startswith('apply_') or key.startswith('abolish_') or key.startswith('config_'):
	    vec = key.split('_')
	    proposals.append((vec[0], vec[1], vec[2], kv['value']))
	elif key.startswith('voter_'):
	    vec = key.split('_')[1:]
	    votes.append((vec[0], vec[1], vec[2], int(kv['value'])))
	elif key == 'kol_reward_distribution':
	    kol_reward_dist = [(k, int(v[0]), v[1], int(v[2])) for k, v in json.loads(kv['value']).items()]
	elif key == 'validator_reward_distribution':
	    validator_reward_dist = [(k, int(v[0]), v[1], int(v[2])) for k, v in json.loads(kv['value']).items()]
	elif key == 'validator_candidates':
	    validator_cands = [(v[0], int(v[1]), v[2]) for v in json.loads(kv['value'])]
	elif key == 'dpos_config':
	    cfg = [(k, str(v)) for k, v in json.loads(kv['value']).items()] 
	elif key == 'committee':
	    committee = [[v] for v in json.loads(kv['value'])]
	elif key == 'kol_candidates':
	    kol_cands = [(k, int(v)) for k, v in json.loads(kv['value'])]
	else:
	    pass
    
    if len(proposals) > 0:
	cur.executemany("INSERT INTO proposal(mode, role, addr, content) VALUES (?,?,?,?)", proposals)
    if len(votes) > 0:
	cur.executemany("INSERT INTO vote(voter, role, candidate, amount) VALUES (?,?,?,?)", votes)
    if len(cfg) > 0:
	cur.executemany("INSERT INTO configuration(key, value) VALUES (?,?)", cfg)
    if len(validator_reward_dist) > 0:
	cur.executemany("INSERT INTO validator_reward_distribution(addr, amount, pool, rate) VALUES (?,?,?,?)", validator_reward_dist)
    if len(kol_reward_dist) > 0:
	cur.executemany("INSERT INTO kol_reward_distribution(addr, amount, pool, rate) VALUES (?,?,?,?)", kol_reward_dist)
    if len(validator_cands) > 0:
	cur.executemany("INSERT INTO validator_candidates(addr, stake, node) VALUES (?,?,?)", validator_cands)
    if len(kol_cands) > 0:
	cur.executemany("INSERT INTO kol_candidates(addr, stake) VALUES (?,?)", kol_cands)
    if len(committee) > 0:
	cur.executemany("INSERT INTO committee(addr) VALUES (?)", committee)
    return

def data_store(data, db_file):
    ''' Write dpos data to sqlite database file '''

    con = lite.connect(db_file) 
    with con:
	cur = con.cursor()
	cur.execute("DROP TABLE IF EXISTS proposal")
	cur.execute("DROP TABLE IF EXISTS configuration")
	cur.execute("DROP TABLE IF EXISTS validator_reward_distribution")
	cur.execute("DROP TABLE IF EXISTS kol_reward_distribution")
	cur.execute("DROP TABLE IF EXISTS validator_candidates")
	cur.execute("DROP TABLE IF EXISTS committee")
	cur.execute("DROP TABLE IF EXISTS kol_candidates")
	cur.execute("DROP TABLE IF EXISTS vote")
	cur.execute("CREATE TABLE proposal(id INTEGER PRIMARY KEY AUTOINCREMENT, mode TEXT, role TEXT, addr TEXT, content TEXT)")
	cur.execute("CREATE TABLE configuration(id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, value TEXT)")
	cur.execute("CREATE TABLE validator_reward_distribution(id INTEGER PRIMARY KEY AUTOINCREMENT, addr TEXT, amount INT, pool TEXT, rate INT)")
	cur.execute("CREATE TABLE kol_reward_distribution(id INTEGER PRIMARY KEY AUTOINCREMENT, addr TEXT, amount INT, pool TEXT, rate INT)")
	cur.execute("CREATE TABLE vote(id INTEGER PRIMARY KEY AUTOINCREMENT, voter TEXT, role TEXT, candidate TEXT, amount INT)")
	cur.execute("CREATE TABLE validator_candidates(id INTEGER PRIMARY KEY AUTOINCREMENT, addr TEXT, stake INT, node TEXT)")
	cur.execute("CREATE TABLE committee(id INTEGER PRIMARY KEY AUTOINCREMENT, addr TEXT)")
	cur.execute("CREATE TABLE kol_candidates(id INTEGER PRIMARY KEY AUTOINCREMENT, addr TEXT, stake INT)")
	data_parse(cur, data['result']['metadatas'])

def data_update(db_file = './dpos.db', url='http://seed1.bumo.io:16002/'):
    ''' Update sqlite database '''
    res, data = data_get(url)
    if not res:
	return False, data
    else:
	data_store(data, db_file)
    return True, 'Store dpos data to sqlite done'

def data_read(table, db_file = './dpos.db'):
    ''' Read data from database '''
    
    if table not in tables:
	return [] 

    con = lite.connect(db_file) 
    with con:
	cur = con.cursor()
	cur.execute('SELECT * from %s limit 1000' % table)
	return cur.fetchall()
if __name__ == "__main__":

        print data_update()
