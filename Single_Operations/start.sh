#!/bin/bash

SCALING_FOR_START_multi="$1"
SCALING_FOR_START_add="$2"
var_start=""

timer_start()
{
    var_start=$(date +%s)
}
timer_end()
{
    var_end=$(date +%s)
    elapsed=$((var_end - var_start))
    hours=$((elapsed / 3600))
    minutes=$(( (elapsed % 3600) / 60 ))
    seconds=$((elapsed % 60))
    printf "Single Operations - took: %02d:%02d:%02d\n" $hours $minutes $seconds
}

chmod +x scripts/*.sh

timer_start

cd scripts;
if [ "$SCALING_FOR_START_add" != "" ]; then
    
    ./run.sh "$SCALING_FOR_START_multi" "$SCALING_FOR_START_add"
else
    clear
    ./run.sh
fi

timer_end
