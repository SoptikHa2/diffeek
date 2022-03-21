#!/bin/bash
# Expected usage: ./$0 $1 $2, $1 is path to file differ binary.
#  and $2 is name (! not path) of the folder with files 
#  named sourceX.txt and x1.txt, x2.txt, etc.
set -euo pipefail

trap 'rm -rf $tmpname' EXIT

realpath="$(readlink -f "$0")"
dirname="$(dirname "$realpath")"
red="\e[91m"
green="\e[92m"
reset="\e[0m"

result=0

fail=0
pass=0

tmpname="$(mktemp -d)"

for file in "$dirname/$2/source"*.txt; do
    letter="${file: -5:1}"
    letter="${letter,}"
    for f in "$dirname/$2/$letter"*.txt; do
        testFailed=0
        if ! ./"$1" "$file" "$f" > "$tmpname"/patch; then
            echo -e "$red""[!]$reset Failure when comparing $file -> $f" >&2
            testFailed=1
            result=1
        fi
        cp "$file" "$tmpname"/s
        cp "$f" "$tmpname"/d
        if ! patch "$tmpname"/s "$tmpname"/patch; then
            echo -e "$red""[!]$reset Failure when patching $file to look like $f" >&2
            testFailed=1
            result=1
        fi
        md1="$(md5sum "$tmpname"/s | cut -d' ' -f1)"
        md2="$(md5sum "$tmpname"/d | cut -d' ' -f1)"
        if [ "$md1" != "$md2" ]; then
            echo -e "$red""[!]$reset FILE WAS NOT PATCHED SUCCESSFULLY: $file -> $f" >&2
            result=1
            testFailed=1
        fi

        if [ $testFailed -eq 1 ]; then
            fail=$((fail+1))
        else
            pass=$((pass+1))
        fi
        rm -rf "${tmpname:?}"/*
    done
done

rm -rf "$tmpname"

echo -e "Failed $red$fail$reset tests."
echo -e "Passed $green$pass$reset tests."

exit "$result"

