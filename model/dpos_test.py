#!/usr/bin/env python
# -- coding: utf-8 --

# dpos_test - buchain function and performance test
# Author: kuangkai@bubi.cn
# Date: 2019-1-18 11:43

import os
import sys
import requests
import getopt
import json
import time
import shutil
import random
import logging
import pdb


base_url = 'http://127.0.0.1:36012/'
max_items = 500 # max tx number per http request
genesis_account = 'buQs9npaCq9mNFZG18qu88ZcmXYqd6bqpTU3'
genesis_priv_key = 'privbvYfqQyG3kZyHE4RX4TYVa32htw8xG4WdpCTrymPUJQ923XkKVbM'

committee_path = '/tmp/committees'
keypairs = './keypairs'
voters_path = '/tmp/voters'
kols_path = '/tmp/kols'
validators_path_prefix = '/tmp/keypairs-bm' # '/tmp/keypairs-d01', '/tmp/keypairs-bm01'

dpos_js = '/root/dpos/bumo/src/contract/dpos.js'
dpos_delegate_js = '/root/dpos/bumo/src/contract/dpos_delegate.js'
dpos_addr = 'buQqzdS9YSnokDjvzg4YaNatcFQfkgXqk6ss'  # addr of dpos_delegate_js
# set if dpos creator account is not genesis account
dpos_creator_account = {"address": genesis_account, "private_key": genesis_priv_key}
# init committee, no need to change
init_committee = {'buQZoJk8bq6A1AtsmfRw3rYJ79eMHUyct9i2': 'privbt8Qg7h2YDDZqMRTuTG34SMzNXREdm8UQUtBpVDHk6e5xA54GTZ2',
                  'buQYKj4TTJPVDPXCLWeBZMoCr1JPhq9Z2tJm': 'privbzxyHQyzHWGK3UbqqPn1aewXoKQ8hFdidLWfroaArT93zjwriDbQ',
                  'buQcYkkoZFMwDNQgCD7DoykNZjtax4FjVSzy': 'privbUJnNYNbN6R6FcCfeght8pPZZdxqFUfsSkaGuoFtjptUxMbVpX7J',
                  'buQmKmaeCyGcPk9KbvnkhpLzQa34tQ9MaWwt': 'privbtd1TQCEYqP4cReoBKnG3Fkv1odmMBKv5RLrYJcyiD8hWw5VVug2'}

commands = [ 
    'th',
    'hello',
    'createAccount',
    'getAccount',
    'getAccountBase',
    'getGenesisAccount',
    'getAccountMetaData',
    'getAccountAssets',
    'debug',
    'getTransactionBlob',
    'getTransactionHistory',
    'getTransactionCache',
    'getContractTx',
    'getStatus',
    'getLedger',
    'getModulesStatus',
    'getConsensusInfo',
    'updateLogLevel',
    'getAddress',
    'getPeerNodeAddress',
    'getLedgerValidators',
    'getPeerAddresses',
    'multiQuery',
    'submitTransaction']


class ChainApi(object):
    ''' Http request interaction with blockchain '''
    
    def __init__(self):
        return
    
    def req(self, module, payload, post=False, sync_wait=False):
        ''' Send http request '''

        cnt = sync_wait and 20 or 1
        for i in xrange(cnt):
            if post:
                r = requests.post(base_url + module, data=json.dumps(payload))
            else:
                r = requests.get(base_url + module, params=payload)
            if r.ok:
                if sync_wait and r.json()['error_code'] != 0:
                    if debug:
                        logger.info('sleep 1 second')
                    time.sleep(1)
                else:
                    return r.json()
            else:
                return None
        return None


    def newNonce(self, acc):
        ''' Get nonce value '''

        res = self.req('getAccount', {'address': acc})
        if res['error_code'] == 0:
            if res['result'].has_key('nonce'):
                return res['result']['nonce'] + 1
            else:
                return 1
        else:
            return None


    def callContract(self, opt_type, input_str, contract_addr="", sync_wait=False):
        ''' Call contract by contract account or payload '''

        payload = {
            "contract_address": contract_addr,
            "code": "",
            "input": input_str,
            "fee_limit": 100000000000,
            "gas_price": 1000,
            "opt_type": opt_type,
            "source_address": ""
        }

        return self.req('callContract', payload, True, sync_wait)


    def createContract(self, acc, nonce, contract='', init_input='', src_account={}):
        ''' Create contract with init input '''
        
        src_addr = ''
        priv_key = ''
        if not src_account:
            src_addr = genesis_account
            priv_key = genesis_priv_key
        else:
            src_addr = src_account['address']
            priv_key = src_account['private_key']

        payload = {
        'items': [{
            'private_keys': [priv_key],
            'transaction_json': {
                'fee_limit': '2000000000',
                'gas_price': 1000,
                'nonce': nonce,
                'operations': [{
                    'create_account': {
                        'dest_address': acc,
                        'init_balance': 10000000,
                        'init_input': init_input,
                         'contract': {
                            'payload': contract
                        },
                        'priv': {
                            'master_weight': 0,
                            'thresholds': {
                                'tx_threshold': '1'
                            }
                        }
                    },
                    'type': 1
                }],
                'source_address': src_addr 
            }
        }]
        }
        return self.req('submitTransaction', payload, post=True)


    def genKeyPairs(self, number, append=False, output=keypairs):
        ''' Generate a specified number of keypairs '''

        start = time.time()
        if not os.path.exists(output):
            os.mknod(output)
        elif not append:
            backup = output + '.bak'
            if os.path.exists(backup):
                os.remove(backup)
            if os.path.exists(output):
                os.rename(output, backup)
                os.mknod(output)

        with open(output, 'r+') as f:
            for i in xrange(number):
                res = self.req('createAccount', {})
                if not res:
                    return False, 'Failed to generate keypair'
                else:
                    account = {}
                    account['address'] = res['result']['address']
                    account['private_key'] = res['result']['private_key']
                    account['private_key_aes'] = res['result']['private_key_aes']
                f.seek(0, 2)
                f.write(json.dumps(account) + '\n')
        if debug:
            logger.info('Generate %s keypairs done in %.2f second' % (number, (time.time() - start)))
        return True, ''


    def addPayload(self, payload, op_type, acc_list, src_acc={},
                   nonce=1, amount=0, input_str=''):
        ''' Add new tx to payload
        Args:
            payload: the payload which tx will be add to
            op_type: type of tx operation
            acc_list: dest account list
            src_acc: address and private key info of source account, ex: {"private_key":xx, "addresss":xx}
            nonce: nonce value of tx, equal to source_account.nonce+1
            amount: the amount value will be use in operation
            input_str: input info when trigger a contract
        '''

        operations = []
        acc_priv_list = []

        if op_type == 'pay_coin':
            for acc in acc_list:
                operations.append({
                        "type": 7,
                        "pay_coin": {
                        "dest_address": acc,
                        "amount": amount,
                        "input": input_str
                        }
                      })
        elif op_type == 'create_account':
            for acc in acc_list:
                operations.append({
                    'create_account': {
                        'dest_address': acc,
                        'init_balance': amount,
                        'priv': {
                            'master_weight': 1,
                            'thresholds': {
                                'tx_threshold': '1'
                            }
                        }
                    },
                    'type': 1
                })
        elif op_type == 'issue_asset':
            for acc in acc_list:
                operations.append({
                    'issue_asset': {
                        "amount": amount,
                        "code": "CNY"
                    },
                    'source_address': acc['address'],
                    'type': 2
                })
                acc_priv_list.append(acc['private_key'])
        else:
            logger.error('Unknown type, %s' % op_type)
            return

        if src_acc:
            src_addr = src_acc['address']
            priv_key = src_acc['private_key']
        else:
            src_addr = genesis_account
            priv_key = genesis_priv_key

        payload['items'].append({
            "transaction_json": {
              "source_address": src_addr,
              "nonce": nonce,
              "fee_limit": op_type == 'issue_asset' and 5050000000 or 200000000,
              "gas_price": 1000,
              "operations": operations
            },
           "private_keys": [priv_key] + acc_priv_list
        })

        return

    def addOperation(self, payload, op_type, dst_addr,
                     src_acc={}, amount=0, input_str=''):
        ''' add a new operation to tx
        Args:
            payload: payload with http request
            op_type: type of tx operation
            dst_addr: dest account address
            src_acc: address and private key info of source account, ex: {"private_key":xx, "addresss":xx}
            amount: the amount value will be use in operation
            input_str: input info when trigger a contract
        '''

        if len(payload['items']) != 1:
            return False, 'payload should contain one tx, got %s' % len(payload['items'])

        operations = payload['items'][0]['transaction_json']['operations']
        acc_priv_list = []

        src_addr = ''
        src_private_key = ''
        if src_acc:
            src_addr = src_acc['address']
            src_private_key = src_acc['private_key']

        if op_type == 'pay_coin':
            operations.append({
                    "type": 7,
                    "pay_coin": {
                    "source_address": src_addr,
                    "dest_address": dst_addr,
                    "amount": amount,
                    "input": input_str
                    }
                  })
        else:
            logger.error('Unknown type, %s' % op_type)
            return

        if src_private_key:
            payload['items'][0]['private_keys'].append(src_private_key)

    def sendRequest(self, payload):
        ''' Divide http request with the global setting max_items '''
    
        success_count = 0
        p = {'items': []}
        for i in xrange(len(payload['items'])):
            if i + 1 % max_items == 0:
                time.sleep(5)
                res = self.req('submitTransaction', p, post=True)
                logger.info(json.dumps(res, indent=4))
                err_list = []
                for err in res['results']:
                    if err['error_code'] != 0:
                        err_list.append(err)
                if len(err_list) > 0:
                    if debug:
                        logger.info(json.dumps(err_list, indent=4))
                    else:
                        pass
                success_count += res['success_count']
                p = {'items': []}
            else:
                p['items'].append(payload['items'][i])
        if len(p['items']) > 0:
            res = self.req('submitTransaction', p, post=True)
            logger.info(json.dumps(res, indent=4))
            err_list = []
            for err in res['results']:
                if err['error_code'] != 0:
                    err_list.append(err)
            if len(err_list) > 0:
                if debug:
                    logger.info(json.dumps(err_list, indent=4))
                else:
                    pass
            success_count += res['success_count']
        return success_count

    def waitTxDone(self, tx_hash):
        ''' Wait transaction apply done'''
        
        cnt = 35
        for i in xrange(cnt):
            tx_res = self.req('getTransactionHistory', {'hash': tx_hash})
            if tx_res['error_code'] != 0:
                logger.info('Wait 1 second for tx apply finish')
                time.sleep(1)
            else:
                break
        if tx_res['error_code'] != 0:
            return False, 'Failed to execute transaction'
        return True, tx_res 

class Performance(ChainApi):
    ''' Do performance test and check '''
    
    def __init__(self):
        ChainApi.__init__(self)
        return
    
    def _profiler(func):
        def func_wrapper(self, *args, **kwargs):
            start_time = time.time()
            res = func(self, *args, **kwargs)
            logger.warn("%s execute take %s seconds" % (func.__name__, time.time() - start_time))
            return res
        return func_wrapper

    def getSpan(self, l, i, j):
        ''' Get slice from list l start from i with number j '''

        if i + j > len(l) - 1:
            return l[i + 1:] + l[:j - (len(l) - 1 - i)]
        else:
            return l[i + 1:i + 1 + j]

    def getTps(self, startSeq=0, endSeq=0):
        ''' Get performance of tx per second
        Args:
            startSeq: start ledger sequence
            endSeq: end ledger sequence
        '''
        
        # get time span from startSeq to endSeq
        if endSeq == 0:
            res = self.req('getLedger', {})
            if res['error_code'] == 0:
                end_time = res['result']['header']['close_time']
                if 'tx_count' not in res['result']['header'].keys():
                    end_num = 0
                else:
                    end_num = res['result']['header']['tx_count']
                endSeq = res['result']['header']['seq']
            else:
                print res
                return
        else:
            res = self.req('getLedger', {'seq':endSeq})
            if res['error_code'] == 0:
                end_time = res['result']['header']['close_time']
                if 'tx_count' not in res['result']['header'].keys():
                    end_num = 0
                else:
                    end_num = res['result']['header']['tx_count']
            else:
                print 'Block %s, %s' % (endSeq, res) 
                return

        if startSeq == 0:
            startSeq = endSeq - 1
        elif startSeq == 1:
            startSeq = 2
        res = self.req('getLedger', {'seq':startSeq})
        if res['error_code'] == 0:
            start_time = res['result']['header']['close_time']
            if 'tx_count' not in res['result']['header'].keys():
                start_num = 0
            else:
                start_num = res['result']['header']['tx_count']
        else:
            print 'Block %s, %s' % (startSeq, res) 
            return 
        
        tx_count = end_num - start_num
        time_span = float((end_time - start_time)) / 1000000
        print 'Block %s-%s, %s txs take %.2f second, tps is: %.2f' % (startSeq, endSeq, tx_count, time_span, tx_count / time_span) 
        return tx_count, time_span

    def dumpLedgerView(self, span, startSeq=2):
        ''' Dump the close time and tx count of every block '''
        
        res = self.req('getLedger', {})
        if res['error_code'] == 0:
            endSeq = res['result']['header']['seq']
        if span == 0:
            span = 8640
        idx = startSeq + span
        lines = []
        while(idx < endSeq and startSeq < endSeq):
            tc, ts = self.getTps(startSeq, idx)
            startSeq += span 
            idx += span
            lines.append('%s %.2f\n' % (tc, ts))
        
        tc, ts = self.getTps(startSeq, endSeq)
        lines.append('%s %.2f\n' % (tc, ts))
        with open('./data.log', 'w') as f:
            f.writelines(lines)

    def testCreateAccount(self, numTx, numOpPerTx):
        ''' Performance test of create account
        Args:
            numTx: total number of accounts to create
            numOpPerTx: number of accounts to create per transaction
        '''

        self.genKeyPairs(numTx * numOpPerTx)

        acc_list = []
        payload = {'items': []}
        n = self.newNonce(genesis_account)
        with open(keypairs, 'r') as f:
            for line in f:
                acc_list.append(json.loads(line.strip())['address'])
                if len(acc_list) == numOpPerTx:
                    self.addPayload(payload, 'create_account', acc_list, nonce=n, amount=5300000000)
                    n += 1
                    acc_list = []
                else:
                    continue

        if debug:
            logger.info(json.dumps(payload, indent=4))

        if acc_list:
            self.addPayload(payload, 'create_account', acc_list, nonce=n, amount=5030000000)
        success_count = self.sendRequest(payload)

        logger.info('Create accounts test done, %s succeed, %s failed' % (success_count, numTx - success_count))
        return


    def testPayCoin(self, numTx, numOpPerTx, start_nonce=0):
        ''' Performance test of pay coin
        Args:
            numTx: total number of accounts to create
            numOpPerTx: number of accounts to create per transaction
            start_nonce: start nonce of source account
        '''

        lines = []
        send_dict = {}
        acc_list = []
        with open(keypairs, 'r') as f:
            lines = f.readlines()

        payload = {'items': []}
        for i in xrange(numTx):
            if not start_nonce:
                m = random.randint(0, len(lines) - 1)
                acc_list = [json.loads(acc)['address'] for acc in self.getSpan(lines, m, numOpPerTx)]
                # increase nonce value if address has been used
                addr_info = json.loads(lines[m])
                addr = addr_info['address']
                if addr in send_dict.keys():
                    send_dict[addr] += 1
                else:
                    send_dict[addr] = self.newNonce(addr)
                nonce = send_dict[addr]
            else:
                idx = i
                if i > (len(lines) - 1):
                    idx = i % len(lines)
                    start_nonce += 1
                acc_list = [json.loads(acc)['address'] for acc in self.getSpan(lines, idx, numOpPerTx)]
                addr_info = json.loads(lines[idx])
                nonce = start_nonce

            self.addPayload(payload, 'pay_coin', acc_list, addr_info, nonce, amount=4900000000)

        success_count = self.sendRequest(payload)

        logger.info('Pay coin test done, %s tx succeed, %s tx failed' % (success_count, numTx - success_count))
        return


    def testIssueAsset(self, numTx, numOpPerTx, start_nonce=0):
        ''' Performance test of issue asset
        Args:
            numTx: total number of accounts to create
            numOpPerTx: number of accounts to create per transaction
            start_nonce: start nonce of source account
        '''

        lines = []
        send_dict = {}
        acc_list = []
        with open(keypairs, 'r') as f:
            lines = f.readlines()

        payload = {'items': []}
        for i in xrange(numTx):
            if not start_nonce:
                m = random.randint(0, len(lines) - 1)
                acc_list = [json.loads(acc) for acc in self.getSpan(lines, m, numOpPerTx)]
                # increase nonce value if address has been used
                addr_info = json.loads(lines[m])
                addr = addr_info['address']
                if addr in send_dict.keys():
                    send_dict[addr] += 1
                else:
                    send_dict[addr] = self.newNonce(addr)
                nonce = send_dict[addr]
            else:
                idx = i
                if i > (len(lines) - 1):
                    idx = i % len(lines)
                    start_nonce += 1
                acc_list = [json.loads(acc) for acc in self.getSpan(lines, idx, numOpPerTx)]
                addr_info = json.loads(lines[idx])
                nonce = start_nonce

            self.addPayload(payload, 'issue_asset', acc_list, addr_info, nonce, amount=100000000)

        success_count = self.sendRequest(payload)

        logger.info('Issue asset test done, %s tx succeed, %s tx failed' % (success_count, numTx - success_count))
        return


class Dpos(ChainApi):
    ''' Dpos initialize and test '''

    def __init__(self):
        ChainApi.__init__(self)
        return

    def _profiler(func):
        def func_wrapper(self, *args, **kwargs):
            start_time = time.time()
            res = func(self, *args, **kwargs)
            logger.warn("%s execute take %s seconds" % (func.__name__, time.time() - start_time))
            return res
        return func_wrapper

    def addressValid(self, addr):
        ''' Validate address '''
        return addr.startswith('buQ') and len(addr) == 36
    
    @_profiler
    def dposInit(self, update=False):
        ''' Create dpos contract and delegate contract '''

        # create committee account
        nonce = self.newNonce(genesis_account)
        payload = {'address': init_committee.keys()[-1]}
        ret = self.req('getAccount', payload)
        if ret['error_code'] != 0:
            payload = {'items': []}
            self.addPayload(payload, 'pay_coin', init_committee.keys(), {}, nonce, amount=100000000000)
            success_count = self.sendRequest(payload)
            if success_count != 1:
                return False, 'Failed to submit apply request'
            nonce += 1

        # check and create dpos contract
        if not update:
            payload = {'address': dpos_addr}
            ret = self.req('getAccount', payload)
            if ret['error_code'] == 0:
                logger.info('Create dpos contract done')
                return True, ''

        logger.info('Start create logic contract')
        res = self.createContract('', nonce, open(dpos_js, 'r').read())
        if res['results'][0]['error_code'] != 0:
            return False, 'Failed to create dpos contract, %s' % res
        else:
            logger.info('Create dpos logic contract done')

        # get dpos contract from tx and create delegate contract
        tx_hash = res['results'][0]['hash']
        if not tx_hash:
            return False, 'Failed to get tx hash, %s' % res
     
        res, tx_res = self.waitTxDone(tx_hash)
        if not res:
            return False, msg
        
        logic_addr = json.loads(tx_res['result']['transactions'][0]['error_desc'])[0]['contract_address']
        if not self.addressValid(logic_addr):
            return False, 'Failed to get logic contract address'

        if update:
            self.updateCfg('logic_contract', logic_addr)
            return True, ''

        try:
            with open(dpos_delegate_js, 'r+') as f:
                content = f.read()
        except Exception as e:
            return False, str(e)

        input_str = "{\"method\": \"init\", \"params\": {\"logic_contract\": \"%s\", \"committee\": [\"buQZoJk8bq6A1AtsmfRw3rYJ79eMHUyct9i2\", \"buQYKj4TTJPVDPXCLWeBZMoCr1JPhq9Z2tJm\", \"buQcYkkoZFMwDNQgCD7DoykNZjtax4FjVSzy\", \"buQmKmaeCyGcPk9KbvnkhpLzQa34tQ9MaWwt\"]}}" % logic_addr
        
        logger.info('Start create dpos delegate contract')
        res = self.createContract(dpos_addr, newNonce(dpos_creator_account['address']), content, input_str, src_account=dpos_creator_account)
        if debug:
            logger.info(json.dumps(res, indent=4))

        if res['results'][0]['error_code'] != 0:
            return False, 'Failed to create dpos delegate contract, %s' % json.dumps(res)
        else:
            logger.info('Create dpos delegate contract done')
        
        with open(committee_path, 'w') as f:
            for c in init_committee.keys():
                f.write(json.dumps({'private_key': init_committee[c], 'address': c}) + '\n')
        time.sleep(10)

        return True, ''

    def getDposCfg(self):
        ''' get dpos configuration by key'''

        input_str = "{\"method\":\"getConfiguration\"}"
        ret = self.callContract(2, input_str, dpos_addr)
        if ret['error_code'] != 0:
            return None

        return json.loads(ret['result']['query_rets'][0]
                          ['result']['value'])['configuration']


    @_profiler
    def apply(self, role_type, accounts):
        ''' Apply as validator, committee or kol
        Args:
            role_type: dpos role type
            account: applicant
        '''

        payload = {'items': []}
        apply_addrs = [acc['address'] for acc in accounts]
        nonce = self.newNonce(genesis_account)
        coin_amount = 0

        cfg = self.getDposCfg()
        if not cfg:
            return False, 'Failed to get dpos configuration'

        if role_type == 'validator':
            coin_amount = int(cfg['validator_min_pledge'])
        elif role_type == 'kol':
            coin_amount = int(cfg['kol_min_pledge'])

        # prepare pledge coin
        self.addPayload(payload, 'pay_coin', apply_addrs, {}, nonce, amount=coin_amount+300000000)
        
        if debug:
            logger.info(json.dumps(payload, indent=4))

        success_count = self.sendRequest(payload)
        if success_count != 1:
            return False, 'Failed to create accounts [%s], success_count:%s' % (','.join(apply_addrs), success_count)

        # check and wait for create voter account done
        payload = {'address': apply_addrs[-1]}
        cnt = 35
        for i in xrange(cnt):
            res = self.req('getAccount', payload, post=False)
            if not res or res['error_code'] != 0:
                logger.info('Wait pledge coin transferred done')
                time.sleep(1)
            elif res['result']['balance'] < coin_amount:
                logger.info('Wait pledge coin transferred done')
                time.sleep(1)
            else:
                break
        if not res or res['error_code'] != 0 or res['result']['balance'] < coin_amount:
            return False, 'Failed to prepare pledge coin'

        payload = {'items': []}
        for acc in accounts:
            nonce = self.newNonce(acc['address'])
            params = "{\"method\": \"apply\", \"params\": {\"role\": \"%s\", \"node\":\"%s\"}}" % (role_type, acc['address'])
            self.addPayload(payload, 'pay_coin', [dpos_addr], acc, nonce, amount=coin_amount, input_str=params)

        if debug:
            logger.info(json.dumps(payload, indent=4))

        success_count = self.sendRequest(payload)
        if success_count != len(accounts):
            return False, 'Failed to submit apply request, success_count:%s != %s' % (success_count, str(len(accounts)))

        return True, ''

    @_profiler
    def vote(self, role_type, accounts, candidates, coin_amount=1000000000):
        ''' User vote for validator candidate, kol candidate
        Args:
            role_type: role_type: dpos role type
            accounts: voter accounts
            candidates: the candidates going to support
            coin_amount: token amount to vote
        '''

        if role_type != 'validator' and role_type != 'kol':
            return False, 'Only validator and kol can be voted'

        # prepare coin for vote
        payload = {'items':[]}
        voter_addrs = [acc['address'] for acc in accounts]
        nonce = self.newNonce(genesis_account)
        loop = 0
        if len(voter_addrs)%100 != 0:
            loop=len(voter_addrs)/100 + 1
        else:
            loop=len(voter_addrs)/100

        for i in xrange(loop):
            if i == loop-1:
                v_addrs = voter_addrs[i * 100:]
            else:
                v_addrs = voter_addrs[i * 100:(i+1)*100]
            self.addPayload(payload, 'pay_coin', v_addrs, {}, nonce, amount=coin_amount+500000000) # 100000000 for tx fee
            nonce += 1
        if debug:
            logger.info(json.dumps(payload, indent=4))
        success_count = self.sendRequest(payload)
        if success_count != loop:
            return False, 'Failed to create voter accounts, success_count: %s' % success_count  

        # check and wait for create voter account done
        cnt = 35
        for i in xrange(cnt):
            new_nonce = self.newNonce(genesis_account)
            if new_nonce != nonce:
                logger.info('Wait 1 second for prepare vote coin done')
                time.sleep(1)
            elif new_nonce == nonce:
                break
        if self.newNonce(genesis_account) != nonce:
            return False, 'Failed to prepare coin for vote, still on processing'

        for i in xrange(len(accounts)):
            accounts[i]['nonce'] = self.newNonce(accounts[i]['address'])

        payload = {'items':[]}
        for i in xrange(len(accounts)):
            idx = i % len(candidates)
            params = "{\"method\": \"vote\", \"params\": {\"role\": \"%s\", \"address\": \"%s\"}}" % (role_type, candidates[idx]) 
            self.addPayload(payload, 'pay_coin', [dpos_addr], accounts[i], accounts[i]['nonce'], amount=coin_amount, input_str=params)

        if debug:
            logger.info(json.dumps(payload, indent=4))
        success_count = self.sendRequest(payload)
        if success_count != len(accounts):
            return False, 'Failed to vote for candidats [%s], success_count:%s' % (','.join(candidates), success_count)

        return True, ''

    @_profiler
    def unVote(self, role_type, accounts, candidates):
        ''' Reduce vote coin amount
        Args:
            role_type: role_type: dpos role type
            accounts: the users of unvote
            candidates: candidates going to unvote from
        '''

        if role_type != 'validator' and role_type != 'kol':
            return False, 'Only validator and kol can be voted'

        for i in xrange(len(accounts)):
            accounts[i]['nonce'] = self.newNonce(accounts[i]['address'])

        payload = {'items':[]}
        for i in xrange(len(accounts)):
            params = "{\"method\": \"unVote\", \"params\": {\"role\": \"%s\", \"address\": \"%s\"}}" % (role_type, candidates[i%len(candidates)])
            self.addPayload(payload, 'pay_coin', [dpos_addr], accounts[i], accounts[i]['nonce'], amount=0, input_str=params)

        if debug:
            logger.info(json.dumps(payload, indent=4))

        success_count = self.sendRequest(payload)
        if success_count != len(accounts):
            return False, 'Failed to unVote for candidates [%s], success_count:%s' % (','.join(candidates), success_count)

        return True, ''

    def withdraw(self, role_type, account):
        ''' Withdraw from validator candidate list or kol candidate
        Args:
            role_type: role_type: dpos role type
            account: the user going to withdraw
        '''

        # apply for withdraw
        payload = {'items':[]}
        apply_addr = account['address']
        nonce = self.newNonce(apply_addr)
        params = "{\"method\": \"withdraw\", \"params\": {\"role\": \"%s\"}}" % role_type

        self.addPayload(payload, 'pay_coin', [dpos_addr], account, nonce, input_str=params)
        if debug:
            logger.info(json.dumps(payload, indent=4))
        success_count = self.sendRequest(payload)
        if success_count != 1:
            return False, 'Failed to submit withdraw request'

        # wait for 15 days (default frozen days), second withdraw apply and extract should be split in 2 operation
        time.sleep(32) # set expiration time to 60s 
        payload = {'items':[]}
        nonce += 1
        params = "{\"method\": \"withdraw\", \"params\": {\"role\": \"%s\"}}" % role_type
        self.addPayload(payload, 'pay_coin', [dpos_addr], account, nonce, input_str=params)
        nonce += 1
        params = "{\"method\": \"extract\"}"
        self.addPayload(payload, 'pay_coin', [dpos_addr], account, nonce, input_str=params)

        if debug:
            logger.info(json.dumps(payload, indent=4))

        success_count = self.sendRequest(payload)
        if success_count != 2:
            return False, 'Failed to submit extract request'

        return True, ''


    def abolish(self, role_type, account, malicious):
        ''' Abolish validator, kol or committee '''

        # one can only abolish member from the same group
        payload = {'items':[]}
        apply_addr = account['address']
        nonce = self.newNonce(apply_addr)
     
        params = "{\"method\": \"abolish\", \"params\": {\"role\": \"%s\", \"address\": \"%s\", \"proof\": \"see abnormal record\"}}" % (role_type, malicious)
        self.addPayload(payload, 'pay_coin', [dpos_addr], account, nonce, input_str=params)
        success_count = self.sendRequest(payload)

        if debug:
            logger.info(json.dumps(payload, indent=4))

        if success_count != 1:
            return False, 'Failed to submit abolish request'

        return True, ''

    def approve(self, operate, item, proposers):
        ''' Committee approve user to join group
        Args:
            operate: apply, abolish or configure
            item: role type of dpos
            proposers: the proposer to approve
        '''

        input_str = "{\"method\":\"getCommittee\"}"
        ret = self.callContract(2, input_str, dpos_addr)
        if ret['error_code'] != 0:
            return None

        committee_addrs = json.loads(ret['result']['query_rets'][0]['result']['value'])['committee']
        
        committee_keypair = []
        with open(committee_path, 'r') as f:
            lines = [json.loads(l.strip()) for l in f]
            committee_keypair = filter(lambda l : l['address'] in committee_addrs, lines) 
            committee_keypair = committee_keypair[:len(committee_keypair)/2 + 1] # pass_rate = 0.5

        # only committee member can approve the applicant
        payload = {'items':[]}
        for member in committee_keypair:
            if member['address'] in proposers:
                continue
            nonce = self.newNonce(member['address'])
            for proposer in proposers:
                params = "{\"method\": \"approve\", \"params\": {\"operate\": \"%s\", \"item\": \"%s\", \"address\": \"%s\"}}" % (operate, item, proposer)
                self.addPayload(payload, 'pay_coin', [dpos_addr], member, nonce, input_str=params)
                nonce += 1
        
        success_count = self.sendRequest(payload)
     
        if debug:
            logger.info(json.dumps(payload, indent=4))

        if success_count < len(committee_keypair) * len(proposers) * 2 / 3:
            return False, 'Failed to submit approve request, total tx:%s, success_count:%s' % (len(committee_keypair) * len(proposers), success_count)
        return True, ''

    def append(self, role_type, accounts):
        ''' Append pledge coin
        Args:
            role_type: dpos role type
            accounts: applicants
        '''

        payload = {'items': []}
        append_addrs = [acc['address'] for acc in accounts]
        nonce = self.newNonce(genesis_account)
        coin_amount = 1000000000000

        # prepare append pledge coin
        self.addPayload(payload, 'pay_coin', append_addrs, {}, nonce, amount=coin_amount+100000000)
        
        if debug:
            logger.info(json.dumps(payload, indent=4))

        success_count = self.sendRequest(payload)
        if success_count != 1:
            return False, 'Failed to prepare coin for [%s], success_count:%s' % (','.join(apply_addrs), success_count)

        # check and wait for create voter account done
        payload = {'address': append_addrs[-1]}
        cnt = 35
        for i in xrange(cnt):
            res = self.req('getAccount', payload, post=False)
            if not res or res['error_code'] != 0:
                logger.info('Wait pledge coin transferred done')
                time.sleep(1)
            elif res['result']['balance'] < coin_amount:
                logger.info('Wait pledge coin transferred done')
                time.sleep(1)
            else:
                break
        if not res or res['error_code'] != 0 or res['result']['balance'] < coin_amount:
            return False, 'Failed to prepare pledge coin'
            
        payload = {'items': []}
        for acc in accounts:
            nonce = self.newNonce(acc['address'])
            params = "{\"method\": \"append\", \"params\": {\"role\": \"%s\"}}" % role_type
            self.addPayload(payload, 'pay_coin', [dpos_addr], acc, nonce, amount=coin_amount, input_str=params)

        if debug:
            logger.info(json.dumps(payload, indent=4))

        success_count = self.sendRequest(payload)
        if success_count != len(accounts):
            return False, 'Failed to submit apply request, success_count:%s' % str(len(accounts))

        return True, ''


    def configure(self, item, value, account):
        ''' Update dpos configuration
        Args:
            item: key of configuration
            value: value of configuration
            account: the proposer
        '''

        # only committee member can proposal new configuration
        payload = {'items':[]}
        apply_addr = account['address']
        nonce = self.newNonce(apply_addr)
        
        if item == 'logic_contract':
            params = "{\"method\": \"configure\", \"params\": {\"item\": \"%s\", \"value\": \"%s\"}}" % (item, value)
        else:
            params = "{\"method\": \"configure\", \"params\": {\"item\": \"%s\", \"value\": %s}}" % (item, value)
        self.addPayload(payload, 'pay_coin', [dpos_addr], account, nonce, input_str=params)
        success_count = self.sendRequest(payload)
        
        if debug:
            logger.info(json.dumps(payload, indent=4))

        if success_count != 1:
            return False, 'Failed to submit configure request'

        return True, ''

    def setVoteDividend(self, role_type, account, pool):
        ''' Set vote reward ratio '''
        
        payload = {'items': []}
        params = "{\"method\": \"setVoteDividend\", \"params\": {\"role\": \"%s\", \"pool\": \"%s\", \"ratio\":80}}" % (role_type, pool)
        self.addPayload(payload, 'pay_coin', [dpos_addr], account, self.newNonce(account['address']), input_str=params)

        if debug:
            logger.info(json.dumps(payload, indent=4))
        success_count = self.sendRequest(payload)
        if success_count != 1: 
            return False, 'Failed to set vote reward ratio'

        return True, ''

    def cleanProposal(self, operate, item, address):
        ''' Clean outdate proposal '''

        payload = {'items': []}
        params = "{\"method\": \"clean\", \"params\": {\"item\": \"%s\", \"address\": \"%s\", \"operate\": \"%s\"}}" % (item, address, operate) 
        account = {'address': genesis_account, 'private_key': genesis_priv_key}
        self.addPayload(payload, 'pay_coin', [dpos_addr], account, self.newNonce(genesis_account), input_str=params)

        if debug:
            logger.info(json.dumps(payload, indent=4))
        success_count = self.sendRequest(payload)
        if success_count != 1:
            return False, 'Failed to clean proposal'

        return True, ''

    def setNodeAddress(self, account, new_node):
        ''' Set node address '''
        
        payload = {'items': []}
        params = "{\"method\": \"setNodeAddress\", \"params\": {\"address\": \"%s\"}" % new_node
        self.addPayload(payload, 'pay_coin', [dpos_addr], account, self.newNonce(account['address']), input_str=params)

        if debug:
            logger.info(json.dumps(payload, indent=4))
        success_count = self.sendRequest(payload)

        if success_count != 1:
            return False, 'Failed to call contract success_count:%s' % success_count
        return True, ''

    def updateCfg(self, item, value, comAccount = {}):
        ''' Update dpos configuration '''

        # proposal a new configuration
        if not comAccount:
            comAddr = init_committee.keys()[0]
            comAccount = {'address': comAddr, 'private_key': init_committee[comAddr]}

        res, msg = self.configure(item, value, comAccount)
        if not res:
            logger.error('Failed to udpate dpos configuration, %s' % msg)
            sys.exit(1)
        else:
            logger.info('configure done')

        # approveCfg
        res, msg = self.approve('config', item, [comAccount['address']])
        if not res:
            logger.error('Failed to approve new configuration, %s' % msg)
            sys.exit(1)
        else:
            logger.info('approve cfg done')
        

class DposTest(Dpos):
    ''' Dpos function test '''
    
    def __init__(self):
        Dpos.__init__(self)
        return

    def _profiler(func):
        def func_wrapper(self, *args, **kwargs):
            start_time = time.time()
            res = func(self, *args, **kwargs)
            logger.warn("%s execute take %s seconds" % (func.__name__, time.time() - start_time))
            return res
        return func_wrapper

    def waitForApproveDone(self, operate, item, address):
        ''' Wait committee approve done '''

        cnt = 35
        proposal = None
        for i in xrange(cnt):
            input_str = "{\"method\":\"getProposal\", \"params\":{\"operate\": \"%s\", \"item\":\"%s\", \"address\":\"%s\"}}" % (operate, item, address)
            ret = self.callContract(2, input_str, dpos_addr)
            proposal = json.loads(ret['result']['query_rets'][0]['result']['value'])['proposal']
            if proposal and (operate == 'penalty' or proposal.has_key('passTime')):
                break
            else:
                logger.info('Wait for proposal done')
                time.sleep(1)
        if not proposal or (operate != 'penalty' and not proposal.has_key('passTime')):
            return False
        return True

    def waitForVoteDone(self, voter_addr, nonce_before):
        ''' Wait for vote done '''

        cnt = 35 
        new_nonce = nonce_before 
        for i in xrange(cnt):
            new_nonce = self.newNonce(voter_addr)
            if new_nonce == nonce_before+1:
                break
            else:
                logger.info('Wait for vote done, nonce:%s != %s' % (new_nonce, nonce_before+1))
                time.sleep(1)
        
        if new_nonce == nonce_before:
            logger.error('Vote transaction still on processing')
            return False
        return True

    def waitForProposalDone(self, operate, item, address):
        ''' Wait for vote done '''

        cnt = 35 
        proposal = None
        for i in xrange(cnt):
            input_str = "{\"method\":\"getProposal\", \"params\":{\"operate\": \"%s\", \"item\":\"%s\", \"address\":\"%s\"}}" % (operate, item, address)
            ret = self.callContract(2, input_str, dpos_addr)
            proposal = json.loads(ret['result']['query_rets'][0]['result']['value'])['proposal']
            if proposal:
                break
            else:
                logger.info('Wait for proposal done')
                time.sleep(1)
        if not proposal:
            return False
        return True

    @_profiler
    def testValidatorElection(self, quit = False):
        ''' Test validator election '''
        
        # init dpos contract
        res, msg = self.dposInit()
        if not res:
            errmsg = 'Failed to init dpos contract, %s' % msg
            return False, errmsg

        # generate voters
        self.genKeyPairs(10, append=True, output=voters_path)
     
        # generate validator candidate 
        self.genKeyPairs(1, append=True, output=keypairs)
        
        # get validator candidates
        input_str = "{\"method\":\"getValidatorCandidates\"}"
        ret = self.callContract(2, input_str, dpos_addr)
        if ret['error_code'] != 0:
            errmsg = 'Failed to call contract, %s' % ret
            return None
        validator_candidates = json.loads(ret['result']['query_rets'][0]['result']['value'])['validator_candidates']

        node_account = {}
        with open(keypairs, 'r') as f:
            lines = [json.loads(l.strip()) for l in f.readlines()]
            candidate_addrs = [item[0] for item in validator_candidates]
            new_lines = filter(lambda l : l['address'] not in candidate_addrs, lines)
            if not new_lines:
                errmsg = 'No enough validator election test keypairs'
                return False, errmsg
            else:
                node_account = new_lines[0]
         
        voter_accounts = []
        with open(voters_path, 'r') as f:
            voter_accounts = [json.loads(l.strip()) for l in f.readlines()]
            if not voter_accounts:
                errmsg = 'No enough voters'
                return False, errmsg
            else:
                voter_accounts = voter_accounts[:10]

        res, msg = self.apply('validator', [node_account])
        if not res:
            errmsg = 'Failed to apply as validator for %s, %s' % (node_account['address'], msg)
            return False, errmsg
        else:
            logger.info('%s apply validator done' % node_account['address'])

        # wait for apply done
        if not self.waitForProposalDone('apply', 'validator', node_account['address']):
            errmsg = 'Application still on processing'
            return False, errmsg
     
        # committee approve applicant
        res, msg = self.approve('apply', 'validator', [node_account['address']])
        if not res:
            errmsg = 'Committee approve validator applicant failed, %s' % msg
            return False, errmsg
        else:
            logger.info('Committee approve validator applicant done')

        
        # wait committee approve done
        if not self.waitForApproveDone('apply', 'validator', node_account['address']):
            errmsg = 'Committee approve still on processing'
            return False, errmsg

        # set vote reward ratio
        pool = 'buQB2VyzA74QjYpjtozimUZk4mEUYQGaTJuf'
        res, msg = self.setVoteDividend('validator', node_account, pool)
        if not res:
            return False, msg

        # vote for the new candidate to make it become validator
        nonce_before = self.newNonce(voter_accounts[-1]['address']) or 0 
        res, msg = self.vote('validator', voter_accounts, [node_account['address']], 1000000000)
        if not res:
            errmsg = 'Failed to vote for validator applicant, %s' % msg
            return False, errmsg
        else:
            logger.info('Users vote for validator %s done' % node_account['address'])
        
        # wait for vote done
        if not self.waitForVoteDone(voter_accounts[-1]['address'], nonce_before):
            errmsg = 'Vote tx still on processing'
            return False, errmsg

        # append pledge coin
        res, msg = self.append('validator', [node_account])
        if not res:
            return False, msg
        else:
            logger.info('Submit append pledge coin done')

        # user reduce vote
        res, msg = self.unVote('validator', voter_accounts, [node_account['address']])
        if not res:
            errmsg = 'Failed to reduce vote for validator applicant, %s' % msg
            return False, errmsg
        else:
            logger.info('Users reduce vote for validator %s done' % node_account['address'])
        
        # wait for vote done
        if not self.waitForVoteDone(voter_accounts[-1]['address'], nonce_before + 1):
            errmsg = 'Vote tx still on processing'
            return False, errmsg

        # set node address
        #newNode = "buQiyrQMA2P1K2kjkfi2adHEFwioVLTdC3A4"
        newNode = node_account['address'] 
        res, errmsg = self.setNodeAddress(node_account, newNode)
        if not res:
            return False, errmsg

        # withdraw
        if quit:
            res, msg = self.withdraw('validator', node_account)
            if not res:
                errmsg = 'Failed to withdraw from validator candidates, %s' % msg
                return False, errmsg
            else:
                logger.info('Validator %s withdraw done' % node_account['address'])
        return True, node_account

    @_profiler
    def testKolElection(self, quit = False):
        ''' Test kol election '''

        # init dpos contract
        res, msg = self.dposInit()
        if not res:
            errmsg = 'Failed to init dpos contract, %s' % msg
            return False, errmsg

        # generate voters
        self.genKeyPairs(10, append=True, output=voters_path)
     
        # generate validator candidate 
        self.genKeyPairs(1, append=True, output=kols_path)
       
        # get kol candidates
        input_str = "{\"method\":\"getKolCandidates\"}"
        ret = self.callContract(2, input_str, dpos_addr)
        if ret['error_code'] != 0:
            errmsg = 'Failed to call contract, %s' % ret
            return None

        kol_candidates = json.loads(ret['result']['query_rets'][0]['result']['value'])['kol_candidates']

        kol_account = {}
        with open(kols_path, 'r') as f:
            lines = [json.loads(l.strip()) for l in f.readlines()]
            candidate_addrs = [item[0] for item in kol_candidates]
            new_lines = filter(lambda l : l['address'] not in candidate_addrs, lines)
            if not new_lines:
                errmsg = 'No enough kol test keypairs'
                return False, errmsg
            else:
                kol_account = new_lines[0]
        
        voter_accounts = []
        with open(voters_path, 'r') as f:
            voter_accounts = [json.loads(l.strip()) for l in f.readlines()]
            if not voter_accounts:
                errmsg = 'No enough voters'
                return False, errmsg
            else:
                voter_accounts = voter_accounts[:10]

        res, msg = self.apply('kol', [kol_account])
        if not res:
            errmsg = 'Failed to apply as kol for %s, %s' % (kol_account['address'], msg)
            return False, errmsg
        else:
            logger.info('%s apply kol done' % kol_account['address'])
        
        # wait for apply done
        if not self.waitForProposalDone('apply', 'kol', kol_account['address']):
            errmsg = 'Application still on processing'
            return False, errmsg

        # committee approve applicant
        res, msg = self.approve('apply', 'kol', [kol_account['address']])
        if not res:
            errmsg = 'Committee approve kol applicant failed, %s' % msg
            return False, errmsg
        else:
            logger.info('Committee approve kol %s done' % kol_account['address'])
        
        # wait committee approve done
        if not self.waitForApproveDone('apply', 'kol', kol_account['address']):
            errmsg = 'Committee approve still on processing'
            return False, errmsg

        # set vote reward ratio
        pool = 'buQB2VyzA74QjYpjtozimUZk4mEUYQGaTJuf'
        res, msg = self.setVoteDividend('kol', kol_account, pool)
        if not res:
            return False, msg

        # vote for the new candidate to make it become validator
        coin_amount = random.randint(1, 5) * 1000000000 
        nonce_before = self.newNonce(kol_account['address'])
        res, msg = self.vote('kol', voter_accounts, [kol_account['address']], coin_amount)
        if not res:
            errmsg = 'Failed to vote for kol, %s' % msg
            return False, errmsg
        
        # wait for vote done
        if not self.waitForVoteDone(voter_accounts[-1]['address'], self.newNonce(voter_accounts[-1]['address'])):
            errmsg = 'Vote tx still on processing'
            return False, errmsg

        # append pledge coin
        res, msg = self.append('kol', [kol_account])
        if not res:
            return False, msg
        else:
            logger.info('Submit append pledge coin done')

        # user reduce vote
        res, msg = self.unVote('kol', voter_accounts, [kol_account['address']])
        if not res:
            errmsg = 'Failed to reduce vote for kol applicant, %s' % msg
            return False, errmsg
        else:
            logger.info('Users reduce vote for kol %s done' % kol_account['address'])

        # withdraw 
        if quit:
            res, msg = self.withdraw('kol', kol_account)
            if not res:
                errmsg = 'Failed to withdraw from kol, %s' % msg
                return False, errmsg
        return True, kol_account

    @_profiler
    def testCommitteeGovernance(self):
        ''' Test configuration election '''

        # init dpos contract
        res, msg = self.dposInit()
        if not res:
            errmsg = 'Failed to init dpos contract, %s' % msg
            return False, errmsg
        
        # generate committee member
        input_str = "{\"method\":\"getCommittee\"}"
        ret = self.callContract(2, input_str, dpos_addr)
        if ret['error_code'] != 0:
            return False, 'Failed to call contract, %s' % ret  

        committee_addrs = json.loads(ret['result']['query_rets'][0]['result']['value'])['committee']

        comAccount = {}
        self.genKeyPairs(1, append=True, output=committee_path)

        with open(committee_path, 'r') as f:
            lines = [json.loads(l.strip()) for l in f.readlines()]
            new_lines = filter(lambda l : l['address'] not in committee_addrs, lines)
            if len(new_lines) < 1:
                errmsg = 'No enough committee test keypairs'
                return False, errmsg
            else:
                comAccount = new_lines[0]

        # apply as committee
        res, msg = self.apply('committee', [comAccount])
        if not res:
            errmsg = 'Failed to apply as committee, %s' % msg
            return False, errmsg
        else:
            logger.info('Apply as committee done')
        
        # wait for apply done
        if not self.waitForProposalDone('apply', 'committee', comAccount['address']):
            errmsg = 'Application still on processing'
            return False, errmsg

        # committee approve applicant
        res, msg = self.approve('apply', 'committee', [comAccount['address']])
        if not res:
            errmsg = 'Failed to approve committee member, %s' % msg
            return False, errmsg
        else:
            logger.info('Approve committee member done')

        # wait committee approve done
        if not self.waitForApproveDone('apply', 'committee', comAccount['address']):
            errmsg = 'Committee approve still on processing'
            return False, errmsg
     
        self.updateCfg('valid_period', 30000000)

        # withdraw
        res, msg = self.withdraw('committee', comAccount)
        if not res:
            errmsg = 'Failed to withdraw from committee, %s' % msg
            return False, errmsg
        return True, ''

    @_profiler
    def testAbolish(self, malicious_addr, role_type='validator'):
        ''' Test abolish validator, kol or committee '''
        
        input_str = ''
        key = ''
        items = ''
        proposer_path = ''
        if role_type == 'validator':
            input_str = "{\"method\":\"getValidatorCandidates\"}"
            key = 'validator_candidates'
            proposer_path = validator_path_prefix + '01'
        elif role_type == 'kol':
            input_str = "{\"method\":\"getKolCandidates\"}"
            key = 'kol_candidates'
            proposer_path = kols 
        elif role_type == 'committee':
            input_str = "{\"method\":\"getCommittee\"}"
            key = 'committee'
            proposer_path = committee_path 
        else:
            errmsg = 'Unknown dpos role_type %s' % role_type
            return False, errmsg

        ret = self.callContract(2, input_str, dpos_addr)
        if ret['error_code'] != 0:
            errmsg = 'Failed to call contract %s with input %s' % (dpos_addr, input_str)
            return False, errmsg 

        items = json.loads(ret['result']['query_rets'][0]['result']['value'])[key]
        if key != 'committee':
            candidate_addrs = [v[0] for v in items]
        else:
            candidate_addrs = items

        if malicious_addr not in candidate_addrs:
            errmsg = 'Malicious not in candidate list'
            return False, errmsg

        if not os.path.exists(proposer_path):
            errmsg = 'No keypairs file in %s' % proposer_path
            return False, errmsg
        
        proposer_account = {}
        with open(proposer_path, 'r') as f:
            lines = [json.loads(l.strip()) for l in f.readlines()]
            new_lines = filter(lambda l : l['address'] in candidate_addrs, lines)
            if len(new_lines) < 1:
                errmsg = 'No candidates keypairs exist'
                return False, errmsg
            else:
                proposer_account = new_lines[0]
        
        if not proposer_account:
            errmsg = 'No candidates keypairs exist'
            return False, errmsg
            
        res, msg = self.abolish(role_type, proposer_account, malicious_addr)
        if not res:
            errmsg = 'Failed to proposal abolish %s, %s' % (role_type, msg)
            return False, errmsg

        # wait for proposal done
        if not self.waitForProposalDone('abolish', role_type, malicious_addr):
            errmsg = 'Application still on processing'
            return False, errmsg
        
        # committee approve applicant
        res, msg = self.approve('abolish', role_type, [malicious_addr])
        if not res:
            errmsg = 'Failed to approve abolish proposal, %s' % msg
            return False, errmsg
        else:
            logger.info('Committtee approve abolish %s done' % malicious_addr)
        return True, ''

    def addCandidates(self, role_type, applicants_path):
        ''' Prepare voting test environment
        Args:
            applicants_path, the path of applicants file
        '''
         
        # get candidates
        if role_type == 'validator':
            input_str = "{\"method\":\"getValidatorCandidates\"}"
        elif role_type == 'kol':
            input_str = "{\"method\":\"getKolCandidates\"}"
        ret = self.callContract(2, input_str, dpos_addr)
        if ret['error_code'] != 0:
            return False, 'Failed to call contract, %s' % ret

        candidates = json.loads(ret['result']['query_rets'][0]['result']['value'])['%s_candidates' % role_type]
        if not os.path.exists(applicants_path):
            return False, 'No such file %s' % applicants_path
        
        accounts = []
        with open(applicants_path, 'r') as f:
            lines = [json.loads(l.strip()) for l in f]
            candidate_addrs = [item[0] for item in candidates]
            accounts = filter(lambda l : l['address'] not in candidate_addrs, lines)
            if len(accounts) == 0:
                return True, 'No pre-candidate accounts'
        
        if debug:
            logger.info('Add candidates [%s]' % ','.join([acc['address'] for acc in accounts]))

        # apply as validator
        res, msg = self.apply(role_type, accounts)
        if not res:
            return False, 'Failed to apply as validators, %s' % msg

        # wait for proposal done
        if not self.waitForProposalDone('apply', role_type, accounts[-1]['address']):
            errmsg = 'Application still on processing'
            return False, errmsg

        # committee approve all application
        res, msg = self.approve('apply', role_type, [acc['address'] for acc in accounts])
        if not res:
            return False, 'Failed to approve applications, %s' % msg

        return True, ''

    def initVote(self, candidate_file=''):
        ''' add all candidates for voting test'''

        # add kols
        res, msg = self.addCandidates('kol', kols_path)
        if not res:
            errmsg = 'Failed to add candidates from %s, %s' % (f, msg)
            return False, errmsg 

        # add candidates
        if not candidate_file:
            candidates_file_list = ['/tmp/keypairs-bm%02d' % i for i in xrange(1, 15)]
        else:
            candidates_file_list = [candidate_file]

        for f in candidates_file_list:
            if not os.path.exists(f):
                errmsg = 'No such candidates file, %s' % f
                continue
            res, msg = self.addCandidates('validator', f)
            if not res:
                errmsg = 'Failed to add candidates from %s, %s' % (f, msg)
                return False, errmsg
            else:
                logger.info('Add candidates from %s done %s' % (f, msg))

        # generate voters
        if not os.path.exists(voters_path):
            self.genKeyPairs(1000, voters_path)
        return True, ''

    def testVote(self, param):
        ''' Test validators update by user vote
        Args:
            param: "mode='normal',voter_num='10',candidate_num=2,init=true",
                mode: 'normal' means random voting to validators and the top 'candidate_num' candidates
                'compete' means replace the tail 'candidate_num' of validators repeatedly
                candidate_num: the head 'candidate_num' of candidates
                voter_num: the number of voter
        '''

        mode = 'normal'
        voter_num = 10
        candidate_num = 0 
        
        if param:
            vec = param.strip().split(',')
            for p in vec:
                k = p.strip().split('=')[0]
                if k == 'mode':
                    mode = p.strip().split('=')[1]
                elif k == 'voter_num':
                    voter_num = int(p.strip().split('=')[1])
                elif k == 'candidate_num':
                    candidate_num = int(p.strip().split('=')[1])
                else:
                    errmsg = 'Unknown params %s' % p
        
        if voter_num == 0:
            return False, 'No voter number set'
        
        # get voters
        voter_accounts = []
        with open(voters_path, 'r') as f:
            voter_accounts = [json.loads(l.strip()) for l in f.readlines()]
            if len(voter_accounts) < int(voter_num):
                errmsg = 'No enough voters, %s < %s, run genKeyPair to add more voters' % (len(voter_accounts), voter_num)
                return False, errmsg
            elif len(voter_accounts) > int(voter_num) * 3:
                idx = random.randint(0, len(voter_accounts)-int(voter_num)-1)
                voter_accounts = voter_accounts[idx:idx+int(voter_num)]
                logger.info('Select voters from index %s to %s' % (idx, idx+int(voter_num)))
            elif len(voter_accounts) > int(voter_num):
                voter_accounts = voter_accounts[:voter_num]

        # get validator candidates
        input_str = "{\"method\":\"getValidatorCandidates\"}"
        ret = self.callContract(2, input_str, dpos_addr)
        if ret['error_code'] != 0:
            errmsg = 'Failed to call contract, %s' % ret
            return False, errmsg
        validator_candidates = json.loads(ret['result']['query_rets'][0]['result']['value'])['validator_candidates']

        cfg = self.getDposCfg()
        if not cfg:
            errmsg = 'Failed to get dpos configuration'
            return False, errmsg
        else:
            logger.info('get dpos config done')
        
        validator_num = cfg['validator_size']
       
        # select len(voter_accounts) of validator candidates
        if candidate_num != 0:
            validator_candidates = validator_candidates[:validator_num+candidate_num]
        
        if mode == 'normal':
            vote_candidates = []
            for i in xrange(len(voter_accounts)):
                m = random.randint(0, len(validator_candidates) - 1)
                vote_candidates.append(validator_candidates[m])
            coin_amount = random.randint(1, 5) * 100000000
            res, msg = self.vote('validator', voter_accounts, [v[0] for v in vote_candidates], coin_amount=1000000000)
            if not res:
                errmsg = 'Failed to vote for candidates, %s' % msg
                return False, errmsg
            else:
                logger.info('Vote for %s candidates done' % len(vote_candidates))
        elif mode == 'compete':
            if len(validator_candidates) <= validator_num:
                errmsg = 'Candidates is not enough to do voting test'
                return False, errmsg

            vote_candidates = validator_candidates[validator_num:]
            index_s = 0
            if validator_num - len(vote_candidates) < 0:
                index_s = 0
                vote_candidates = validator_candidates[validator_num:2*validator_num]
            else:
                index_s = validator_num - len(vote_candidates)
            
            coin_amount = int(validator_candidates[index_s][1]) - int(vote_candidates[-1][1]) 
            logger.info('coin amount is : %s - %s = %s' % (int(validator_candidates[index_s][1]), int(vote_candidates[-1][1]), coin_amount))
            candidate_addrs = [v[0] for v in vote_candidates]
            res, msg = self.vote('validator', voter_accounts, candidate_addrs, coin_amount)
            if not res:
                errmsg = 'Failed to vote for [%s], %s' % (','.join([v[0] for v in vote_candidates]), msg)
                return False, errmsg
            else:
                logger.info('Vote for %s candidates done' % len(vote_candidates))

    def testUnVote(self, param):
        ''' unVote all votes, clean vote '''
        
        voter_num = 10
        candidate_num = 1

        if param:
            vec = param.strip().split(',')
            for p in vec:
                k = p.strip().split('=')[0]
                if k == 'voter_num':
                    voter_num = int(p.strip().split('=')[1])
                elif k == 'candidate_num':
                    candidate_num = int(p.strip().split('=')[1])
                else:
                    errmsg = 'Unknown params %s' % p
        
        if voter_num == 0:
            return False, 'No voter number set'

        # get validator candidates
        input_str = "{\"method\":\"getValidatorCandidates\"}"
        ret = self.callContract(2, input_str, dpos_addr)
        if ret['error_code'] != 0:
            errmsg = 'Failed to call contract, %s' % ret
            return False, errmsg

        validator_candidates = json.loads(ret['result']['query_rets'][0]['result']['value'])['validator_candidates']
        
        # get voters
        voter_accounts = []
        with open(voters_path, 'r') as f:
            voter_accounts = [json.loads(l.strip()) for l in f.readlines()]
            if len(voter_accounts) < int(voter_num):
                errmsg = 'No enough voters, %s < %s, run genKeyPair to add more voters' % (len(voter_accounts), voter_num)
                return False, errmsg
            elif len(voter_accounts) > int(voter_num) * 3:
                idx = random.randint(0, len(voter_accounts)-int(voter_num)-1)
                voter_accounts = voter_accounts[idx:idx+int(voter_num)]
                logger.info('Select voters from index %s to %s' % (idx, idx+int(voter_num)))
            elif len(voter_accounts) > int(voter_num):
                voter_accounts = voter_accounts[:voter_num]

        # user reduce vote
        for i in xrange(candidate_num):
            idx = random.randint(0, len(validator_candidates)-1)
            res, msg = self.unVote('validator', voter_accounts, [validator_candidates[idx][0]])
            if not res:
                logger.error('Reduce vote for %s error, %s' % (validator_candidates[idx][0], msg))
                continue
            else:
                v_addrs = ','.join([v['address'] for v in voter_accounts])
                logger.info('Users [%s] unVote for %s done' % (v_addrs, validator_candidates[idx][0]))

            # wait for vote done
            voter_addr = voter_accounts[-1]['address']
            if not self.waitForVoteDone(voter_addr, self.newNonce(voter_addr)):
                errmsg = 'Vote tx still on processing'
                return False, errmsg
        return True, ''


def usage():
    u = '''
    Name:
        %s - bumo python api test
    Synopsis:
        %s -c [command] [options...]
    Description:
        Arguments are as following:
            -h  print the help message
            -c  command
            -f  keypairs file path
            -U  upgrade dpos contract, work with -c init

    Example:
        %s -c genKeyPairs|testPayCoin|testCreateAccount|testIssueAsset -n number [-o numOpPerTx] [-s startNonce] [-f keypairs]
        %s -c dumpLedgerView -n span [-o startSeq]
        %s -c getTps -n startSeq [-o endSeq]
        %s -c init|updateCfg|testValidatorElection|testKolElection|testCommittee [-f keypairs] [-U] [-p key=value]
        %s -c dposTest|testAbolish|cleanProposal [-p malicious|item,operate,address]
        %s -c initVote|testVote|testUnVote [-p 'mode=compete,voter_num=5,candidate_num=5']
        %s -c str2Hex|hex2Str -p raw_string|hex_string
        %s -c getModulesStatus|getAccount|getLedger|getTransactionHistory|list
                
    '''
    prog = os.path.basename(sys.argv[0])
    print 'Usage :'
    print u % (prog, prog, prog, prog, prog, prog, prog, prog, prog, prog)
    sys.exit(0)


def listCommands():
    ''' List all valid commands '''
    
    print "Commands as follow:\n%s" % '\n'.join(['\t' + c for c in commands])

def hex2Str(hex_str):
    ''' hex string to raw string '''
    
    if not hex_str or len(hex_str) % 2 != 0:
        return ""

    raw_str = ''
    for i in xrange(0, len(hex_str), 2):
        raw_str += chr(int(hex_str[i:i+2], 16))
    return raw_str

def str2Hex(raw_str):
    ''' raw string to hex string '''
    hex_str = ''
    for c in raw_str:
        hx_str += hex(ord(c))[2:]
    return hex_str

def dposTest():
    ''' Dpos full function test '''
    
    # validator election
    dt = DposTest()
    res, validator_account = dt.testValidatorElection()
    if not res:
        logger.error(validator_account)
        sys.exit(1)
    # abolish
    res, msg = dt.testAbolish(validator_account['address'], 'validator')
    if not res:
        logger.error(msg)
        sys.exit(1)

    # wait committee approve done
    if not dt.waitForApproveDone('penalty', 'validator', validator_account['address']):
	logger.error('Committee approve validator abolish still on processing')
        sys.exit(1)
    else:
        logger.info('Committee approve validator abolish done')

    # kol election
    res, kol_account = dt.testKolElection()
    if not res:
        logger.error(kol_account)
        sys.exit(1)
    res, msg = dt.testAbolish(kol_account['address'], 'kol')
    if not res:
        logger.error(msg)
        sys.exit(1)

    # wait committee approve done
    if not dt.waitForApproveDone('penalty', 'kol', kol_account['address']):
	logger.error('Committee approve kol abolish still on processing')
        sys.exit(1)
    else:
        logger.info('Committee approve kol abolish done')
    
    # committee governance
    res, msg = dt.testCommitteeGovernance()
    if not res:
        logger.error(msg)
        sys.exit(1)
    
    keys = init_committee.keys()
    proposer = {'address': keys[0], 'private_key': init_committee[keys[0]]}
    res, msg = dt.abolish('committee', proposer, keys[-1])
    if not res:
        logger.error(msg)
        sys.exit(1)
    else:
        logger.info('Committee member %s abolish %s' % (keys[0], keys[-1]))
    
    # wait for proposal outdate(valid_period=30s) and clean 
    time.sleep(35)
    res, msg = dt.cleanProposal('abolish', 'committee', keys[-1])
    if not res:
        logger.error('Failed to clean committee abolish proposal, %s' % msg)
    else:
        logger.info('Clean proposal abolish_committee_%s' % keys[-1])


if __name__ == "__main__":

    '''logging.basicConfig(format='%(asctime)s - %(pathname)s[line:%(lineno)d] - %(levelname)s: %(message)s',
                        level=logging.INFO,
			filename='dpos.log',
			filemode='a')
    '''
    logger = logging.getLogger()
    logger.setLevel('INFO')
    BASIC_FORMAT = "%(asctime)s - %(pathname)s:[line:%(lineno)d] - %(levelname)s: %(message)s"
    DATE_FORMAT = '%Y-%m-%d %H:%M:%S'
    formatter = logging.Formatter(BASIC_FORMAT, DATE_FORMAT)
    chlr = logging.StreamHandler() # to console
    chlr.setFormatter(formatter)
    chlr.setLevel('INFO')
    fhlr = logging.FileHandler('dpos.log') # to log file
    fhlr.setFormatter(formatter)
    logger.addHandler(chlr)
    #logger.addHandler(fhlr)

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hc:p:P:n:o:s:u:f:l:r:qdU")
    except getopt.GetoptError as msg:
        print msg
        sys.exit(1)

    get_cmds = [
        'getAccount',
        'getGenesisAccount',
        'getTransactionHistory',
        'getStatus',
        'getLedger',
        'getModulesStatus',
        'getConsensusInfo',
    ]

    cmd = ''
    para = ''
    url = ''
    role = ''
    post = False
    test = False
    update = False
    debug = False
    quit = False
    apply_fnode = False
    numberN = 0
    numberO = 0
    start_nonce = 0

    for op, arg in opts:
        if op == '-h':
            usage()
        elif op == '-c':
            cmd = arg
        elif op == '-P':
            post = True
        elif op == '-p':
            para = arg
        elif op == '-n':
            numberN = int(arg)
        elif op == '-o':
            numberO = int(arg)
        elif op == '-s':
            start_nonce = int(arg)
        elif op == '-u':
            url = arg.strip('/') + '/'
            base_url = 'http://' + url
        elif op == '-f':
            keypairs = arg
        elif op == '-r':
            role = arg
        elif op == '-U':
            update = True
        elif op == '-d':
            debug = True
        elif op == '-q':
            quit = True
        else:
            print 'Unknown options %s' % op
            sys.exit(1)

    payload = {}
    get_request = lambda module, payload={}: json.dumps(requests.get(base_url + module, params=payload).json(), indent=4)
    post_request = lambda module, payload={}: json.dumps(requests.post(base_url + module, data=payload).json(), indent=4)
    dt = DposTest()
    pt = Performance()

    if cmd == 'list':
        listCommands()
    elif cmd == 'genKeyPairs':
        dt.genKeyPairs(numberN, output=keypairs)
    elif cmd == 'testPayCoin':
        pt.testPayCoin(numberN, numberO, start_nonce)
    elif cmd == 'testCreateAccount':
        pt.testCreateAccount(numberN, numberO)
    elif cmd == 'testIssueAsset':
        pt.testIssueAsset(numberN, numberO, start_nonce)
    elif cmd == 'getTps':
        pt.getTps(numberN, numberO)
    elif cmd == 'dumpLedgerView':
        pt.dumpLedgerView(numberN, numberO)
    elif cmd == 'testValidatorElection':
        print dt.testValidatorElection(quit)
    elif cmd == 'testKolElection':
        print dt.testKolElection(quit)
    elif cmd == 'testCommittee':
        print dt.testCommitteeGovernance()
    elif cmd == 'testAbolish':
        print dt.testAbolish(para, role)
    elif cmd == 'initVote':
        print dt.initVote(para)
    elif cmd == 'dposTest':
        dposTest()
    elif cmd == 'testVote':
        dt.testVote(para)
    elif cmd == 'testUnVote':
        print dt.testUnVote(para)
    elif cmd == 'str2Hex':
        print str2Hex(para)
    elif cmd == 'hex2Str':
        print hex2Str(para)
    elif cmd == 'cleanProposal':
        vec = para.strip().split(',')
        if len(vec) != 3:
            print 'paras error: operate,item,address'
            sys.exit(1)
        dt.cleanProposal(vec[0], vec[1], vec[2])
    elif cmd == 'init':
        print dt.dposInit(update)
    elif cmd == 'updateCfg':
	vec = para.strip().split('=')
	if len(vec) != 2:
	    print 'Failed to parse parameters, %s' % para
	dt.updateCfg(vec[0], vec[1])
    elif cmd in commands:
        if cmd == 'th':
            cmd = 'getTransactionHistory'
        para_json = {}
        if para:
            if '{' in para:
                try:
                    para_json = json.loads(para)
                except ValueError as msg:
                    print 'Failed to parse json string, %s' % msg
                    sys.exit(1)
            elif '=' in para:
                for i in para.split(','):
                    para_json[i.split('=')[0]] = i.split('=')[1]
        if post:
            print post_request(cmd, para_json)
        else:
            print get_request(cmd, para_json)
    else:
	print 'Support commands: ', commands
        print 'No such command: %s' % cmd
        sys.exit(1)
