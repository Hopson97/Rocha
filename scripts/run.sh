#!/bin/bash

if [ "$1" = "release" ]
then
    ./bin/release/RochaTest 
else
    ./bin/debug/RochaTest 
fi