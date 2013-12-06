#!/bin/sh

set -x

make && ./serialServer -d -v -n 192.168.0.143 -p 4001
