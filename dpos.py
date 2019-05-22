#!/usr/bin/python -Eu
# coding: utf-8

import web
import os
import sys
import json
import socket
import commands
import sqlite3

currentdir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, currentdir + '/model')
from dpos_data import data_read, data_update

db_file = currentdir + '/dpos.db'
web_port = 36010


class index:

    def GET(self):
        v_cands=data_read('validator_candidates', db_file)
        k_cands=data_read('kol_candidates', db_file)
        committee=data_read('committee', db_file)
        votes=data_read('vote', db_file)
        proposals=data_read('proposal', db_file)
        v_rewards=data_read('validator_reward_distribution', db_file)
        k_rewards=data_read('kol_reward_distribution', db_file)
        cfg=data_read('configuration', db_file)

        return render.index(v_cands=v_cands,
                            k_cands=k_cands,
                            committee=committee,
                            votes=votes,
                            proposals=proposals,
                            v_rewards=v_rewards,
                            k_rewards=k_rewards,
                            cfg=cfg)


class update:
    
    def GET(self):
        res, msg = data_update(db_file)
        if not res:
            return 'Failed to update dpos data to db, %s' % msg
        else:
            return msg
urls = (
    '/', 'index',
    '/update', 'update',
)

render = web.template.render(os.path.dirname(
    __file__) + '/statics/', cache=False)


class WebSite(web.application):

    def run(self, port=8075, *middleware):
        func = self.wsgifunc(*middleware)
        return web.httpserver.runsimple(func, ('0.0.0.0', port))


# Init app.
app = WebSite(urls, globals())

if __name__ == '__main__':
    res, msg = data_update(db_file, url='http://seed1.bumo.io:16002/')
    if not res:
        print 'Failed to update dpos data to db, %s' % msg
    else:
        print msg

    rcode, msg = commands.getstatusoutput("ifconfig|grep 'inet '")
    ip = msg.split()[1].split('/')[0].strip()
    print 'Please use the browser to open: \n\033[32m\033[4m' + 'http://' + ip + ':' + str(web_port) + '\033[0m'

    # Perform the first fork.
    try:
        pid = os.fork()
        if pid > 0:
            sys.exit(0)  # first parent out
    except OSError as e:
        sys.stderr.write("fork #1 failed: (%d) %s\n" % (e.errno, e.strerror))
        sys.exit(1)

    # Perform the second fork
    try:
        pid = os.fork()
        if pid > 0:
            sys.exit(0)  # second parent out
    except OSError as e:
        sys.stderr.write("fork #2 failed: (%d) %s]n" % (e.errno, e.strerror))
        sys.exit(1)

    file = open(currentdir + '/dpos.log', 'w+')
    os.dup2(file.fileno(), sys.stderr.fileno())
    os.dup2(file.fileno(), sys.stdout.fileno())
    app.run(web_port)
