#!/bin/bash

#Write to a driver
echo "This is a test - PASSED" > /dev/dummydriver 

#Read from a device created by driver
head -n 1 /dev/dummydriver
