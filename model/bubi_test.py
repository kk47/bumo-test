import os
import sys
import getopt
import json
import logging
import requests
import time

bubi_url = 'http://127.0.0.1:19333/'
keypairs = 'keypairs'
mw_keypairs = './mwkeypairs'
mw_contract = 'D:/bubi-v3/src/privacy/mw_token.js'
debug = False
mw_contract_addr = ''
max_items=100

genesis_account = 'a0015544fbf3e4038d9e752c3236b5185f5d98eb56049a'
genesis_priv_key = 'c00109c8d07d81f3b4297f54bf85c0ce57ea5d8c37e48b8be723cfd64d0f63f363138c'


class ChainApi(object):
    ''' Http request interaction with blockchain '''

    def __init__(self, url=''):
        self.url = url or base_url
        return

    def req(self, module, payload, post=False, sync_wait=False):
        ''' Send http request '''

        cnt = sync_wait and 20 or 1
        for i in range(cnt):
            if post:
                r = requests.post(self.url + module, data=json.dumps(payload))
            else:
                r = requests.get(self.url + module, params=payload)
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
            if 'nonce' in res['result']:
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

    def createContract(self, acc, nonce, contract='', src_account={}):
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
        if os.path.exists(output):
            os.remove(output)

        with open(output, 'w+') as f:
            for i in range(number):
                res = self.req('createAccount', {})
                if not res:
                    return False, 'Failed to generate keypair'
                else:
                    account = {}
                    account['address'] = res['result']['address']
                    account['private_key'] = res['result']['private_key']
                    account['private_key_aes'] = res['result']['private_key_aes']
                #f.seek(0, 2)
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
        elif op_type == 'payment':
            for acc in acc_list:
                operations.append({
                    "type": 3,
                    "payment": {
                        "dest_address": acc,
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
        for i in range(len(payload['items'])):
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
        for i in range(cnt):
            tx_res = self.req('getTransactionHistory', {'hash': tx_hash})
            if tx_res['error_code'] != 0:
                logger.info('Wait 1 second for tx apply finish')
                time.sleep(1)
            else:
                break
        if tx_res['error_code'] != 0:
            return False, 'Failed to execute transaction'
        return True, tx_res

class PrivacyTest(ChainApi):
    ''' Do mimblewimble function test '''

    def __init__(self):
        ChainApi.__init__(self, url = bubi_url)
    
    def createMwKeyPair(self, output, append=False):
        self.genKeyPairs(5, output=keypairs)

        if os.path.exists(output):
            os.remove(output)

        with open(output, 'w+') as f:
            for i in range(4):
                res = self.req('createMwKeyPair', {})
                if not res:
                    return False, 'Failed to generate keypair'
                else:
                    account = {}
                    account['priv_key'] = res['result']['priv_key']
                    account['pub_key'] = res['result']['pub_key']
                #f.seek(0, 2)
                f.write(json.dumps(account) + '\n')
        return True, ''

    def getMwToken(self, spend_key, value, to):
        payload = {
            "priv_key": spend_key,
            "value": value,
            "owner": to
        }
        return self.req('createMwToken', payload)

    def createMwTx(self, spend_key, from_addr, to_addr, value, contract_addr):
        payload = {
            "spend_key": spend_key,
            "from":from_addr,
            "to": to_addr,
            "value":value,
            "contract_addr":contract_addr
        }

        return self.req('createMwTx1', payload)

    def issue(self, nonce, amount, token, mw_contract_addr, src_acc={}):
        payload = {'items': []}
        mw_token = "{\"commit\":\"%s\",\"range_proof\":\"%s\",\"public_key\":\"%s\",\"encrypt_value\":\"%s\"}" % (token['commit'], token['rangeproof'], token['from_pub'], token['encrypt_value'])
        input = "{\"method\":\"issue\",\"params\":{\"name\": \"MimbleWimble\",\"symbol\": \"MWT\",\"token\":%s}}" % mw_token
        self.addPayload(payload, 'payment', [mw_contract_addr], src_acc, nonce, input_str=input)
        #print(json.dumps(payload, indent=4))
        success_count = self.sendRequest(payload)
        if success_count != 1:
            return False, 'Failed to submit issue request'
        return True, ''

    def initMw(self):

        self.createMwKeyPair(mw_keypairs)
        logger.info("Generate keypairs done")

        acc_list = []
        mw_list = []

        with open(keypairs, 'r') as f:
            acc_list = [json.loads(l.strip()) for l in f.readlines()]
        with open(mw_keypairs, 'r') as f:
            mw_list = [json.loads(l.strip()) for l in f.readlines()]

        payload = {'items': []}
        n = self.newNonce(genesis_account)
        self.addPayload(payload, 'create_account', [item['address'] for item in acc_list[1:]], nonce=n)
        success_count = self.sendRequest(payload)
        if success_count != 1:
            return False, 'Failed to submit create account request, success_count:%s' % success_count

        # check and wait for create account done
        payload = {'address': acc_list[1]['address']}
        cnt = 25
        for i in range(cnt):
            res = self.req('getAccount', payload)
            if not res or res['error_code'] != 0:
                logger.info('Wait create account done')
                time.sleep(1)
            else:
                break
        if not res or res['error_code'] != 0:
            return False, 'Failed to create account'
        else:
            logger.info('Create account done')

        mw_contract_addr = acc_list[0]['address'] # acc_list[0] will be contract address
        try:
            with open(mw_contract, 'r') as f:
                content = f.read()
        except Exception as e:
            return False, str(e)
        nonce = self.newNonce(acc_list[1]['address'])
        res = self.createContract(mw_contract_addr, nonce, content, src_account=acc_list[1]) # acc_list[1] will be issue and map to mw_list[0]
        if not res:
            logger.info("Failed to create contract, %s" % json.dumps(res, indent=4))
            return False, "Failed to create contract"
        else:
            logger.info("Create contract done, %s" % json.dumps(res, indent=4))

        # wait create contract done
        tx_hash = res['results'][0]['hash']
        if not tx_hash:
            return False, 'Failed to get tx hash, %s' % res

        res, tx_res = self.waitTxDone(tx_hash)
        if not res:
            return False, tx_res


        res = self.getMwToken(mw_list[0]['priv_key'], 10000, mw_list[0]['pub_key'])
        if not res:
            return False, 'Failed to create mw token'
        else:
            logger.info('Create mw token result %s' % json.dumps(res, indent=4))

        res, msg = self.issue(nonce+1, 10000, res['result'], mw_contract_addr, src_acc=acc_list[1])
        if not res:
            return False, 'Failed to issue token %s' % msg
        else:
            logger.info('Issue mw token done')

        addr_pubs = []
        for i in range(len(mw_list)):
            pair = {}
            pair['pub_key'] = mw_list[i]['pub_key']
            pair['address'] = acc_list[i+1]['address']
            addr_pubs.append(pair)
        res, msg = self.addressMapping(self.newNonce(genesis_account), addr_pubs, mw_contract_addr)
        if not res:
            return False, msg
        else:
            logger.info('Address mapping done')
        return True, ''

    def transfer(self, tx, mw_contract_addr, src_acc={}):
        payload = {'items': []}
        tmp = '['
        for t in tx['inputs']:
            token = "{\"id\":\"%s\"}" % t['id']
            tmp += token + ','
        inputs = tmp.rstrip(',')
        inputs += ']'

        tmp = '['
        for t in tx['outputs']:
            token = "{\"commit\":\"%s\", \"range_proof\":\"%s\", \"encrypt_value\":\"%s\", \"from_pub\":\"%s\", \"to_pub\": \"%s\"}" % (t['commit'], t['range_proof'], t['encrypt_value'], t['from_pub'], t['to_pub'])
            tmp += token + ','
        outputs = tmp.rstrip(',')
        outputs += ']'

        input = "{\"method\":\"transfer\", \"params\": {\"excess_sig\": \"%s\", \"inputs\":%s, \"outputs\":%s, \"to\":\"%s\"}}" % (tx['excess_sig'], inputs, outputs, tx['to'])
        nonce = self.newNonce(src_acc['address'])
        self.addPayload(payload, 'payment', [mw_contract_addr], src_acc, nonce, input_str=input)
        success_count = self.sendRequest(payload)
        if success_count != 1:
            return False, 'Failed to submit apply request'
        return True, ''

    def transferTest(self):
        ''' transfer case: 1-1-1, 1 input and 1 output and 1 change, mw_list[0] to mw_list[1]'''
        acc_list = []
        mw_list = []

        with open(keypairs, 'r') as f:
            acc_list = [json.loads(l.strip()) for l in f.readlines()]
        with open(mw_keypairs, 'r') as f:
            mw_list = [json.loads(l.strip()) for l in f.readlines()]

        # get issue token
        issue_token = self.getMwToken(mw_list[0]['priv_key'], 10000, mw_list[0]['pub_key'])
        if not issue_token:
            return False, 'Failed to create issue token'
        else:
            print('Get issue token result %s' % json.dumps(issue_token, indent=4))

        input_token = {}
        input_token['from_pub'] = issue_token['result']['from_pub']
        input_token['commit'] = issue_token['result']['commit']
        input_token['encrypt_value'] = issue_token['result']['encrypt_value']
        mw_contract_addr = acc_list[0]['address']

        res = self.createMwTx(mw_list[0]['priv_key'], acc_list[1]['address'], acc_list[2]['address'], 5000, mw_contract_addr) # acc_list[2] <--> mw_list[1]
        if 'result' not in res:
            logger.error(json.dumps(res, indent=4))
            return False, 'Failed to create mw transaction'

        res['result'].pop('verify_excess')
        #logger.info("Get tx done, %s" % json.dumps(res, indent=4))


        # acc_list[1] call contract
        res, msg = self.transfer(res['result']['params'], mw_contract_addr, acc_list[1])
        if not res:
            return False, msg

        return True, ''

    def decryptToken(self, spend_key, evalue, from_pub):
        payload = {
            "priv_key": spend_key,
            "encrypt_value": evalue,
            "from": from_pub
        }
        return self.req('decryptValue', payload)

    def getToken(self, contract, key):
        payload = {
            "address": contract,
            "key": key
        }
        return self.req('getAccountMetaData', payload)

    def getBalance(self, address):
        acc_list = []
        mw_list = []

        with open(keypairs, 'r') as f:
            acc_list = [json.loads(l.strip()) for l in f.readlines()]
        with open(mw_keypairs, 'r') as f:
            mw_list = [json.loads(l.strip()) for l in f.readlines()]

        mw_contract_addr = acc_list[0]['address']
        res = self.getToken(mw_contract_addr, address)
        if 'result' not in res:
            return False, 'Failed to get token info of issuer'

        #print(json.dumps(res, indent=4))
        tokens = json.loads(res['result'][address]['value'])['tokens']

        spend_key = ''
        for i in range(len(acc_list)):
            if(acc_list[i]['address'] == address):
                spend_key = mw_list[i-1]['priv_key']
                break
        for t in tokens:
            res = self.decryptToken(spend_key, t['encrypt_value'], t['from_pub'])
            logger.info(json.dumps(res, indent=4))
        return True, ''

    def splitToken(self):
        ''' transfer case: 1-2, 1 input and 1 output and 1 change, mw_list[1] to mw_list[1]'''
        acc_list = []
        mw_list = []

        with open(keypairs, 'r') as f:
            acc_list = [json.loads(l.strip()) for l in f.readlines()]
        with open(mw_keypairs, 'r') as f:
            mw_list = [json.loads(l.strip()) for l in f.readlines()]

        mw_contract_addr = acc_list[0]['address']
        key = acc_list[2]['address'] # get mw_list[1]'s token
        res = self.getToken(mw_contract_addr, key)
        if 'result' not in res:
            return False, 'Failed to get token info of issuer'

        input_tokens = json.loads(res['result'][key]['value'])['tokens']
        res = self.createMwTx(mw_list[1]['priv_key'], acc_list[2]['address'], acc_list[2]['address'], 3000, mw_contract_addr)  # acc_list[2] <--> mw_list[1]
        if 'result' not in res:
            logger.error(json.dumps(res, indent=4))
            return False, 'Failed to create mw transaction'

        res['result'].pop('verify_excess')
        # logger.info("Get tx done, %s" % json.dumps(res, indent=4))

        # acc_list[2] call contract
        res, msg = self.transfer(res['result']['params'], mw_contract_addr, acc_list[2])
        if not res:
            return False, msg
        return True, ''

    def transferTest1(self):
        ''' transfer case: 2-1-1, 2 input and 1 output and 1 change, mw_list[1] to mw_list[2]'''
        acc_list = []
        mw_list = []

        with open(keypairs, 'r') as f:
            acc_list = [json.loads(l.strip()) for l in f.readlines()]
        with open(mw_keypairs, 'r') as f:
            mw_list = [json.loads(l.strip()) for l in f.readlines()]

        mw_contract_addr = acc_list[0]['address']
        key = acc_list[2]['address'] # get mw_list[1]'s token
        res = self.getToken(mw_contract_addr, key)
        if 'result' not in res:
            return False, 'Failed to get token info of issuer'

        input_tokens = json.loads(res['result'][key]['value'])['tokens']
        res = self.createMwTx(mw_list[1]['priv_key'], acc_list[2]['address'], acc_list[3]['address'], 4000, mw_contract_addr)  # acc_list[2] <--> mw_list[1]
        if 'result' not in res:
            logger.error(json.dumps(res, indent=4))
            return False, 'Failed to create mw transaction'

        res['result'].pop('verify_excess')
        # logger.info("Get tx done, %s" % json.dumps(res, indent=4))

        # acc_list[2] call contract
        res, msg = self.transfer(res['result']['params'], mw_contract_addr, acc_list[2])
        if not res:
            return False, msg
        return True, ''

    def transferTest2(self):
        ''' transfer case: 1-1,  input and 1 output and 0 change, mw_list[2] to mw_list[3]'''
        acc_list = []
        mw_list = []

        with open(keypairs, 'r') as f:
            acc_list = [json.loads(l.strip()) for l in f.readlines()]
        with open(mw_keypairs, 'r') as f:
            mw_list = [json.loads(l.strip()) for l in f.readlines()]

        mw_contract_addr = acc_list[0]['address']
        key = acc_list[3]['address'] # get mw_list[2]'s token
        res = self.getToken(mw_contract_addr, key)
        if 'result' not in res:
            return False, 'Failed to get token info of issuer'

        input_tokens = json.loads(res['result'][key]['value'])['tokens']
        res = self.createMwTx(mw_list[2]['priv_key'], acc_list[3]['address'], acc_list[4]['address'], 4000, mw_contract_addr)  # acc_list[4] <--> mw_list[3]
        if 'result' not in res:
            logger.error(json.dumps(res, indent=4))
            return False, 'Failed to create mw transaction'

        res['result'].pop('verify_excess')
        # logger.info("Get tx done, %s" % json.dumps(res, indent=4))

        # acc_list[2] call contract
        res, msg = self.transfer(res['result']['params'], mw_contract_addr, acc_list[3])
        if not res:
            return False, msg
        return True, ''

    def addressMapping(self, nonce, addr_pubs, mw_contract_addr):
        payload = {'items': []}
        tmp_nonce = nonce
        for m in addr_pubs:
            input = "{\"method\":\"address_mapping\", \"params\":{\"address\":\"%s\", \"public_key\":\"%s\"}}" % (m['address'], m['pub_key'])
            self.addPayload(payload, 'payment', [mw_contract_addr], {}, tmp_nonce, input_str=input)
            tmp_nonce += 1
        #print(json.dumps(payload, indent=4))
        success_count = self.sendRequest(payload)
        if success_count != len(addr_pubs):
            return False, 'Failed to submit apply request'
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
                initMw:         create mw contract, issue and do mw address mapping
                transfer1-1-1:  one input and two output, include one change
                split:          one token split to two token
                transfer2-1-1:  two input and 2 output, include one change
                transfer1-1:    one input and one output
                getBalance:     get balance of mw token

    Example:
        %s -c transfer1-1-1|split|transfer2-1-1|transfer1-1
        %s -c str2Hex|hex2Str -p raw_string|hex_string
        %s -c getModulesStatus|getAccount|getLedger|getTransactionHistory|list

    '''
    prog = os.path.basename(sys.argv[0])
    print
    'Usage :'
    print(u % (prog, prog, prog, prog, prog, prog, prog, prog, prog, prog))

    sys.exit(0)
if __name__ == "__main__":
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
    logger.addHandler(fhlr)

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hc:p:")
    except getopt.GetoptError as msg:
        print(msg)
        sys.exit(1)

    for op, arg in opts:
        if op == '-h':
            usage()
        elif op == '-c':
            cmd = arg
        elif op == '-p':
            params = arg
        elif op == '-u':
            url = arg.strip('/') + '/'
            base_url = 'http://' + url
        else:
            print('Unknown options %s' % op)
            sys.exit(1)

    pt = PrivacyTest()
    if cmd == "initMw":
        print(pt.initMw())
    elif cmd == "transfer1-1-1":
        print(pt.transferTest())
    elif cmd == "split":
        print(pt.splitToken())
    elif cmd == "address_mapping":
        print(pt.addressMapping())
    elif cmd == "getBalance":
        print(pt.getBalance(params))
    elif cmd == "transfer2-1-1":
        print(pt.transferTest1())
    elif cmd == "transfer1-1":
        print(pt.transferTest2())
    else:
        print("Unknown command %s" % cmd)
        sys.exit(1)
