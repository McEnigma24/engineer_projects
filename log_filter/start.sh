#!/bin/bash

var_start=""
var_end=""
dir_input="input"
dir_build="build"
dir_exe="exe"
dir_log="log"
dir_run_time_config="run_time_config"
SCRIPT="./run.sh"
LOG="../output/start.log"

create_dir() { if [ ! -d $1 ]; then mkdir $1; fi; }

clear_dir() { if [ -d $1 ]; then rm -rf $1; fi; mkdir $1; }
clear_file() { if [ -f $1 ]; then rm -rf $1; fi; }

timer_start() { var_start=$(date +%s); }
timer_end() { var_end=$(date +%s); }

timer_print()
{
    elapsed=$((var_end - var_start))
    hours=$((elapsed / 3600))
    minutes=$(( (elapsed % 3600) / 60 ))
    seconds=$((elapsed % 60))
    printf "Program          - took: %02d:%02d:%02d\n" $hours $minutes $seconds
}
env_prep()
{
    create_dir "$dir_input"
    create_dir "$dir_build"
    create_dir "$dir_exe"
    create_dir "$dir_log"
    create_dir "$dir_run_time_config"

    chmod +x scripts/*.sh

    clear
    cd scripts
}

#####################   START   #####################

env_prep

timer_start
{
    script -q -c "$SCRIPT 2>&1" /dev/null | tee $LOG
}
timer_end

timer_print
