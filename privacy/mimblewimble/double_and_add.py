#!/usr/bin/env python

import os
import sys

def bits(n):
    
    while n:
        yield n & 1
        n >>= 1

def double_and_add(n, x):

    result = 0
    addend = x

    for bit in bits(n):
        if bit == 1:
            result += addend
        addend *= 2
    return result

if __name__ == "__main__":
    
    print double_and_add(151, 1231312312321312)
