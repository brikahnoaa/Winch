#!/bin/bash
awk '/bytes read$/ {sum+=$5} END {print sum-46}' $1
