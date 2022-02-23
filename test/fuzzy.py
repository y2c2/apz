#!/usr/bin/env python3
"""Fuzzy test for apz"""

import sys
import random
import subprocess
from past.utils import old_div

# to get 'past' use:
# pip3 install future

TESTER_NAME = './test'
BIT_CNT_MAX_DEFAULT = 1024

def test_by_bit_cnt(operation, opfunc, lhs, rhs):
    """test_by_bit_cnt"""
    cmd = [TESTER_NAME, operation, str(lhs), str(rhs), str(opfunc(lhs, rhs))]
    proc = subprocess.run(cmd, stdout=subprocess.PIPE, check=True)
    output = proc.stdout
    if len(output) != 0:
        print(lhs)
        print(rhs)
        print(opfunc(lhs, rhs))
        print(str(opfunc(lhs, rhs)))
        print(' '.join(cmd))
        print(output.decode('utf-8'))

def c_rule_int_div(lhs, rhs):
    """c_rule_int_div"""
    return old_div(lhs, rhs)

def test(bit_cnt_max):
    """test"""
    op_possible = [
        ('add', lambda l, r: l + r),
        ('sub', lambda l, r: l - r),
        ('mul', lambda l, r: l * r),
        ('div', c_rule_int_div),
        ('mod', lambda l, r: l % r),
        ]
    operation = random.choice(op_possible)
    bit_cnt_lhs = random.randrange(1, bit_cnt_max)
    bit_cnt_rhs = random.randrange(1, bit_cnt_max)
    lhs = random.getrandbits(bit_cnt_lhs)
    while True:
        rhs = random.getrandbits(bit_cnt_rhs)
        if rhs != 0:
            break
    test_by_bit_cnt(operation[0], operation[1], lhs, rhs)

def main():
    """main"""
    bit_cnt_max = BIT_CNT_MAX_DEFAULT
    if len(sys.argv) == 2:
        bit_cnt_max = int(sys.argv[1])
    for _ in range(500):
        test(bit_cnt_max)

if __name__ == '__main__':
    main()
