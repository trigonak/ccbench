#!/bin/bash
#!/bin/bash

./ccbench $@;
read;
./ccbench $@ -y8;
