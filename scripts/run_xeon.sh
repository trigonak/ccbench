#!/bin/bash

./ccbench $@ -x3;
read;
./ccbench $@ -y11 -x3;
read;
./ccbench $@ -y41 -x3;
