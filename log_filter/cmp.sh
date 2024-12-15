#!/bin/bash

var_start=""
var_end=""
dir_input="input"
dir_build="build"
dir_exe="exe"
dir_log="log"
dir_run_time_config="run_time_config"
dir_R="Ray_Tracer"
dir_S="Single_Operations"
SCRIPT="./run.sh"
LOG="../output/start.log"

# Ścieżki do katalogów
input_dir="input"  # Podmień na właściwą ścieżkę
output_dir="output"  # Podmień na właściwą ścieżkę

# Flaga błędu
error_flag=0

# Iteracja po plikach w katalogu input
find "$input_dir" -type f | while read -r input_file; do
  # Wyznaczenie ścieżki odpowiadającego pliku w output
  relative_path="${input_file#$input_dir/}"
  output_file="$output_dir/$relative_path"

  # Sprawdzenie, czy plik w output istnieje
  if [[ ! -f "$output_file" ]]; then
    echo "Brak pliku w output: $output_file"
    error_flag=1
    continue
  fi

  # Porównanie plików
  # if ! cmp -s "$input_file" "$output_file"; then
  if ! diff -s "$input_file" "$output_file"; then
    echo "Pliki różnią się: $input_file i $output_file"
    error_flag=1
  fi
done

# Podsumowanie
if [[ $error_flag -eq 0 ]]; then
  echo "Wszystkie pliki są identyczne!"
else
  echo "Znaleziono różnice lub brakujące pliki."
fi
