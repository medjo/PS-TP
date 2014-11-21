#!/bin/bash
make
make run &
gdb kernel.bin
