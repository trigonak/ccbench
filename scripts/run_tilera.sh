#!/bin/bash

./run ./ccbench $@ ;
read;
./run ./ccbench $@ -y35;
