#!/bin/bash

file_name="code_base"

dir_tmp="tmp_dir_for_code_base_preparation"
dir_R="Ray_Tracer"
dir_S="Single_Operations"
file_start_all="start_all.sh"

mkdir $dir_tmp

cp $file_start_all $dir_tmp
cp -r $dir_R $dir_tmp
cp -r $dir_S $dir_tmp



cd $dir_tmp

# reset build
rm -rf $dir_R/build/*
rm -rf $dir_S/build/*

# delete checksums
rm -f $dir_R/src/__checksums__.txt
rm -f $dir_S/src/__checksums__.txt

# delete CUDA files
rm -f $dir_R/src/CUDA/*
rm -f $dir_S/src/CUDA/*

# delete Ray Tracing cmp
rm -rf $dir_R/cmp

tar -czf ../$file_name.tar.gz * > /dev/null

cd ..
rm -rf $dir_tmp


echo -e "\nOPEN with      tar -xpzf code_base.tar.gz; ./start_all.sh;\n"
