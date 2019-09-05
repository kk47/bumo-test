#!/usr/bin/env python

from dpos_test import ChainApi

bubi_url = 'http://127.0.0.1:19333'
keypair = './keypairs'
mw_keypair = './mwkeypairs'

genesis_account = 'a0015544fbf3e4038d9e752c3236b5185f5d98eb56049a'
genesis_priv_key = 'c00109c8d07d81f3b4297f54bf85c0ce57ea5d8c37e48b8be723cfd64d0f63f363138c'

class PrivacyTest(ChainApi):
    ''' Do mimblewimble function test '''

    def __init__(self):
        ChainApi.__init__(url = bubi_url)
    
    def createMwKeyPair(self, output):
        self.genKeyPairs(10, output=mw_keypair)
        
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
                res = self.req('createMwKeyPair', {})
                if not res:
                    return False, 'Failed to generate keypair'
                else:
                    account = {}
                    account['priv_key'] = res['result']['priv_key']
                    account['pub_key'] = res['result']['pub_key']
                f.seek(0, 2)
                f.write(json.dumps(account) + '\n')
        if debug:
            logger.info('Generate %s keypairs done in %.2f second' % (number, (time.time() - start)))
        return True, ''
        
    def createMwTx(self):

    def issue(self):
        
    def initMw(self):
        
        acc_list = []
        payload = {'items': []}
        n = self.newNonce(genesis_account)
        with open(keypair, 'r') as f:
            acc_list = [json.loads(l.strip()) for l in f.readlines()]
               
        self.addPayload(payload, 'create_account', acc_list, nonce=n) 

        res, msg = self.createContract()
        if not res:
            return False, msg

        res, msg = self.issue()
        if not res:
            return False, msg

        res, msg = self.addressMapping()
        if not res:
            return False, msg


    def transfer(self):

    def addressMapping(self):
    
    def getAccountInfo(self):


