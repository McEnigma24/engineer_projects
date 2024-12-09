#!/bin/bash

DIR_ROOT=$(pwd)

REP_NUMBER=50
# rep_multiply_param=0.6
rep_multiply_param=0.585

pure_product_name="FINAL_PRODUCT"

dir_R="Ray_Tracer"
dir_S="Single_Operations"
dir_R_scripts="${dir_R}/scripts"
dir_S_scripts="${dir_S}/scripts"
dir_final="ALL_LOGS"
dir_run_time_config="_run_time_config_"
dir_nvidia_cmake_test="nvidia_cmake_test"

path_estimated_finish_date="${dir_run_time_config}/estimated_finish_date.txt"
path_one_rep_duration="${dir_run_time_config}/one_rep_duration.txt"
path_rep_number="${dir_run_time_config}/rep_number.txt"

path_DONE_installed="${dir_run_time_config}/DONE_installed.txt"
path_DONE_user_check_nvidia_bash="${dir_run_time_config}/DONE_user_check_bash.txt"
path_DONE_user_check_nvidia_cmake="${dir_run_time_config}/DONE_user_check_cmake.txt"
path_DONE_build_projects="${dir_run_time_config}/DONE_build_projects.txt"

path_Cmake_TEST="${dir_nvidia_cmake_test}/CMakeLists.txt"


var_start_ALL=""
var_end_ALL=""
var_start_RAY_TRACER=""
var_end_RAY_TRACER=""
var_start_SINGLE_OPERATIONS=""
var_end_SINGLE_OPERATIONS=""

create_dir() { if [ ! -d $1 ]; then mkdir $1; fi; }

clear_dir() { if [ -d $1 ]; then rm -rf $1; fi; mkdir $1; }
clear_file() { if [ -f $1 ]; then rm -rf $1; fi; }

timer_start_ALL() { var_start_ALL=$(date +%s); }
timer_end_ALL() { var_end_ALL=$(date +%s); }

timer_start_RT() { var_start_RAY_TRACER=$(date +%s); }
timer_end_RT() { var_end_RAY_TRACER=$(date +%s); }

timer_start_SO() { var_start_SINGLE_OPERATIONS=$(date +%s); }
timer_end_SO() { var_end_SINGLE_OPERATIONS=$(date +%s); }

print_RT()
{
    elapsed=$((var_end_RAY_TRACER - var_start_RAY_TRACER))
    hours=$((elapsed / 3600))
    minutes=$(( (elapsed % 3600) / 60 ))
    seconds=$((elapsed % 60))
    printf "Ray Tracer        - took: %02d:%02d:%02d\n" $hours $minutes $seconds
}
print_SO()
{
    elapsed=$((var_end_SINGLE_OPERATIONS - var_start_SINGLE_OPERATIONS))
    hours=$((elapsed / 3600))
    minutes=$(( (elapsed % 3600) / 60 ))
    seconds=$((elapsed % 60))
    printf "Single Operations - took: %02d:%02d:%02d\n" $hours $minutes $seconds
}
print_ALL()
{
    elapsed=$((var_end_ALL - var_start_ALL))
    hours=$((elapsed / 3600))
    minutes=$(( (elapsed % 3600) / 60 ))
    seconds=$((elapsed % 60))
    printf "ALL               - took: %02d:%02d:%02d\n" $hours $minutes $seconds
}
DONE()
{
    echo "DONE" > $1
}

install_packages()
{
    # Funkcja sprawdzająca czy pakiet jest zainstalowany
    check_and_install()
    {
        PACKAGE=$1
        if ! dpkg-query -W -f='${Status}' "$PACKAGE" 2>/dev/null | grep "install ok installed" > /dev/null; then
            echo "$PACKAGE is not installed. Installing..."

            sudo apt install -y "$PACKAGE"

            if [ $? -eq 0 ]; then
                echo ""
            else
                echo -e "\nstart_all.sh - ERROR - unable to install this package: $PACKAGE\n"
                exit
            fi
        fi
    }

    # Lista pakietów do zainstalowania
    PACKAGES=(
        tar
        make
        cmake
        build-essential
        gcc
        g++
        libstdc++-11-dev
        gcc-multilib
        g++-multilib
        mesa-utils
        # nvcc
        # nvidia-smi
        nvidia-cuda-toolkit
    )

    # # for 32-bit
    # if ! dpkg --print-foreign-architectures | grep -q "i386"; then
    #     echo "Adding i386 architecture..."
    #     sudo dpkg --add-architecture i386
    # else
    #     echo "i386 architecture is already added."
    # fi

    # Aktualizacja listy pakietów
    sudo apt update -y && sudo apt upgrade -y

    # Sprawdzanie i instalowanie każdego pakietu
    for PACKAGE in "${PACKAGES[@]}"; do
        check_and_install "$PACKAGE"
    done


    echo -ne "\n\n"
    echo "Instalation completed"
    echo -ne "\n\n"
}
start()
{
    cd $DIR_ROOT; cd "$1" > /dev/null
    chmod +x start.sh
    ./start.sh "$2" "$3"
}
check_path()
{
    if [[ "$PWD" == *" "* ]]; then
        echo 'start_all.sh - ERROR: path contains " "      change incorrect dirs'
        exit
    fi
}
user_check_nvidia_bash()
{
    cpu_n=$(lscpu | grep "Model name" | awk -F: '{print $2}' | sed 's/^ *//')
    cpu_name=$(echo $cpu_n | tr '[:space:]-()@.' '_')

    gpu_n=$(nvidia-smi -L | awk -F': ' '{print $2}' | awk -F'(' '{print $1}')
    gpu_name=$(echo $gpu_n | tr '[:space:]-()@.' '_')

    # gpu_n=$(glxinfo | grep "Device" | awk -F'[()]' '{print $2}')
    # gpu_name=$(echo $gpu_n | tr '[:space:]-()@.' '_')

    if echo "$gpu_name" | grep -iq "nvidia"; then

        echo ""

    else
        
        gpu_name="nvidia_gpu_not_present"

    fi

    echo -e "Is this your hardware ?\n"
    echo "CPU: $cpu_name"
    echo "GPU: $gpu_name"

    echo -en "\ntype 'n' if hardware is incorrect OR press 'enter' to approve "
    read user_choice

    if [ "" != "$user_choice" ]; then
    {
        echo "start_all.sh - ERROR: user reported incorrect hardware read";

        nvidia-smi > /dev/null
        if [ $? -eq 0 ]; then
        {
            echo -e "\nstart_all.sh - ERROR - unknown reason -> nvidia-smi IS GOOD"
        }
        else
        {
            echo -e "\nstart_all.sh - ERROR - reason -> nvidia-smi not responding"
        }
        fi

        exit;
    }
    fi;

    clear
}
user_check_nvidia_cmake()
{
    mkdir $dir_nvidia_cmake_test

    # Tworzę fake CMakeLists.txt

    echo 'cmake_minimum_required(VERSION 3.18)' > $path_Cmake_TEST
    echo 'project(MyProject LANGUAGES CXX)' >> $path_Cmake_TEST

    echo 'find_program(NVIDIA_SMI_EXECUTABLE nvidia-smi)' >> $path_Cmake_TEST

    echo 'if (NVIDIA_SMI_EXECUTABLE)' >> $path_Cmake_TEST
    echo 'message(WARNING "nvidia GPU detected")' >> $path_Cmake_TEST
    echo 'else()' >> $path_Cmake_TEST
    echo 'message(WARNING "NO nvidia GPU detected")' >> $path_Cmake_TEST
    echo 'endif()' >> $path_Cmake_TEST

    cd $dir_nvidia_cmake_test

    cmake . 2> nvidia_cmake_test.log > /dev/null

    if grep -q "NO nvidia GPU detected" nvidia_cmake_test.log; then 

        gpu_n=$(nvidia-smi -L | awk -F': ' '{print $2}' | awk -F'(' '{print $1}')
        gpu_name=$(echo $gpu_n | tr '[:space:]-()@.' '_')
        
        # gpu_n=$(glxinfo | grep "Device" | awk -F'[()]' '{print $2}')
        # gpu_name=$(echo $gpu_n | tr '[:space:]-()@.' '_')

        if echo "$gpu_name" | grep -iq "nvidia"; then

            # BASH DETECTED NVIDIA

            # błąd -> tylko jeśli bash złapał nazwę Nvidia w Hardwarze ale CMake nie
            echo "start_all.sh - ERROR: CMake does not detect Nvidia -> nvidia-smi not responding";

            cd ..
            rm -rf $dir_nvidia_cmake_test
            exit;
        fi
    fi

    cd ..
    rm -rf $dir_nvidia_cmake_test
}
build_projects()
{
    clear

    cd $dir_R_scripts
    echo -e "Building - Ray Tracer\n"
    ./production.sh "log_to_terminal"
    if [ $? -eq 0 ]; then
        echo "SUCCESS"
    else
        echo -e "\nstart_all.sh - ERROR - unable to build Ray Tracer"
        exit
    fi
    cd - > /dev/null

    clear

    cd $dir_S_scripts
    echo -e "Building - Single Operations\n"
    ./production.sh "log_to_terminal"
    if [ $? -eq 0 ]; then
        echo "SUCCESS"
    else
        echo -e "\nstart_all.sh - ERROR - unable to build Single Operations"
        exit
    fi
    cd - > /dev/null
}
set_rep_number()
{
    echo $1 > $path_rep_number
}
DRY_RUN()
{
    clear
    total_time=""

    if [ -f $path_one_rep_duration ]; then
    {
        total_time=$(<"$path_one_rep_duration")
    }
    else
    {
        echo "DRY RUN"

        rm -f $path_estimated_finish_date
        set_rep_number "1"
        timer_start_ALL
        {
            start "$dir_S" "0.5" "0"
            start "$dir_R" "0.5" "0.5"
        }
        timer_end_ALL
        print_ALL

        cd $DIR_ROOT;
        total_time=$((var_end_ALL - var_start_ALL))

        echo $total_time > $path_one_rep_duration
    }
    fi

    estimated_time=$((total_time * REP_NUMBER))
    estimated_time=$(echo "scale=6; $rep_multiply_param*$estimated_time" | bc)
    estimated_time=${estimated_time%.*}

    test_end_date=$(date -d "+$estimated_time seconds" +"%Y %m %d %H %M %S")

    echo $test_end_date > $path_estimated_finish_date
}
BENCHMARK_RUN()
{
    echo "BENCHMARK RUN"

    set_rep_number "$REP_NUMBER"
    echo -e "\nEach operations is repeated ( $REP_NUMBER ) times\n";    

    timer_start_ALL
    {
        timer_start_SO
        {
            start "$dir_S" "0.5" "0"
        }
        timer_end_SO

        timer_start_RT
        {
            start "$dir_R" "0.5" "0.5"
        }
        timer_end_RT
    }
    timer_end_ALL

    echo -e "\n"
    print_RT
    print_SO
    print_ALL
}
packin()
{
    echo -e "\nFINAL PACKING\n"
    cd $DIR_ROOT;
    mkdir $dir_final
    mkdir $dir_final/$dir_R
    mkdir $dir_final/$dir_S

    # copy outputs
    cp $dir_R/output/*.tar $dir_final/$dir_R
    cp $dir_S/output/*.tar $dir_final/$dir_S

    # clearing previous products
    find . -maxdepth 1 -type f -name "${pure_product_name}*" -delete

    # packing outputs
    cd $dir_final
    final="../${pure_product_name}_$(whoami)"
    tar -czf $final.tar.gz * > /dev/null

    # clearing tmp dir
    cd $DIR_ROOT;
    rm -rf $dir_final

    echo -e "\n !!! ALL DONE !!! \n\n\n"
    echo -e "[all I need]"
    echo -e "${pure_product_name}_$(whoami).tar.gz"
    
    echo -e "\nand put it here -> \t\thttps://drive.google.com/drive/folders/1h6f_gm76vASG99E2h_5sYujxkXeb8RQA?usp=sharing\n"

    echo -e "\njeśli nazwa jest już zajęta zmień ją albo wybierz opcję (zachowaj oba pliki)\n\n"
}

###########################    START   ##############################

check_path

create_dir "$dir_run_time_config"
if [ ! -f $path_DONE_installed ];   then install_packages; fi; DONE "$path_DONE_installed"
if [ ! -f $path_DONE_user_check_nvidia_bash ];  then user_check_nvidia_bash; fi; DONE "$path_DONE_user_check_nvidia_bash"
if [ ! -f $path_DONE_user_check_nvidia_cmake ];  then user_check_nvidia_cmake; fi; DONE "$path_DONE_user_check_nvidia_cmake"
if [ ! -f $path_DONE_build_projects ];  then build_projects; fi; DONE "$path_DONE_build_projects"
echo -e "\nAll good, beginning the run"


DRY_RUN
BENCHMARK_RUN

packin
