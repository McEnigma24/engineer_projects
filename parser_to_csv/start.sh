#!/bin/bash

DONT_CLEAR="$1"
DIR_SCRIPTS="scripts"
DIR_INPUT="input"
DIR_PD="_PACKED_DATA_"
DIR_R="Ray_Tracer"
DIR_S="Single_Operations"
DIR_SER="serialized"
DIR_SER_M="${DIR_SER}/main"
DIR_SER_S="${DIR_SER}/single"
DIR_TMP="tmp_dir"
var_start=""

path_input_check="${DIR_SCRIPTS}/input_checksum.txt"
path_TMP_input_check="${DIR_SCRIPTS}/TMP_input_checksum.txt"

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
    printf "Parser - took: %02d:%02d:%02d\n" $hours $minutes $seconds
}

clear_dir() { if [ -d $1 ]; then rm -rf $1; fi; mkdir $1; }
create_dir() { if [ ! -d $1 ]; then mkdir $1; fi; }
env_prep()
{
    chmod +x $DIR_SCRIPTS/*.sh

    create_dir "$DIR_SER"
    create_dir "$DIR_SER_M"
    create_dir "$DIR_SER_S"
}
unpack_logs()
{
    cd $DIR_INPUT
    rm -f $DIR_PD/*Zone.Identifier

    for packed_file in $DIR_PD/*; do
    {
        result=${packed_file##*/}               # Usuwa wszystko do ostatniego '/'
        result=${result#FINAL_PRODUCT_}         # Usuwa wszystko przed "FINAL_PRODUCT_"
        result=${result%.tar.gz}                # Usuwa wszystko po ".tar.gz"
        echo "$result"

        mkdir $DIR_TMP;
        tar -xzf $packed_file -C $DIR_TMP > /dev/null;

        cd $DIR_TMP;

        ### MAIN ###
        cd $DIR_R; tar -xf *.tar > /dev/null; rm *.tar
        for single_unpacked_log_file in *; do
        {
            cp $single_unpacked_log_file ../../$DIR_R/$result.log
        }
        done
        cd ..

        ### SINGLE_OP ###
        cd $DIR_S; tar -xf *.tar > /dev/null; rm *.tar
        for single_unpacked_log_file in *; do
        {
            cp $single_unpacked_log_file ../../$DIR_S/$result.log
        }
        done
        cd ..

        cd ..
        rm -rf $DIR_TMP
    }
    done

    cd ..
}
creat_input_checksum()
{
    cd "$DIR_INPUT"
    # Tworzenie listy hashów plików, posortowanej
    find . -name "*.log" -exec sha256sum {} + | sort > ../"$1"
    cd - > /dev/null
}
reset_serialized_hash_maps_if_input_changed()
{
    if [ ! -f "$path_input_check" ]; then
        # Jeśli plik checksum nie istnieje, utwórz go
        creat_input_checksum "$path_input_check"
    else
        # Tworzenie tymczasowego checksumu
        creat_input_checksum "$path_TMP_input_check"

        # Wykrywanie zmian (różnica w plikach lub zawartości)
        if ! diff -q "$path_input_check" "$path_TMP_input_check" > /dev/null; then
            echo -e "\n\nZmiany wykryte -> tworzenie nowych hash_tables\n\n"
            
            # Wyczyść i zaktualizuj checksum oraz serializowane dane
            clear_dir "$DIR_SER"
            mv "$path_TMP_input_check" "$path_input_check"
        else
            echo -e "\n\nBrak zmian -> używanie istniejących hash_tables\n\n"
        fi

        # Usuń tymczasowy plik
        rm -f "$path_TMP_input_check"
    fi
}
parser()
{
    cd ..
    echo ""
    echo -n "Creating demo charts - "
    python3 src/charter.py
    echo -e "DONE\n"
}

############################################################################

# START #

if [ "$DONT_CLEAR" == "" ]; then
    clear
fi

env_prep

cd $DIR_INPUT
clear_dir "$DIR_R"
clear_dir "$DIR_S"
cd ..

unpack_logs

reset_serialized_hash_maps_if_input_changed

cd $DIR_SCRIPTS

timer_start
if [ "$DONT_CLEAR" != "" ]; then
    ./run.sh "dont clear"
else
    ./run.sh
fi

parser
timer_end