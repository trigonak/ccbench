#!/bin/bash

./ccbench $@ ;
read;
./ccbench $@ -y6;
read;
./ccbench $@ -y12;
read;
./ccbench $@ -y18;
