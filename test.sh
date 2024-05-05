#!/bin/bash
assert(){
    # ""はエスケープ($,`,\を除く)
    regex="$1"
    input="$2"
    expected="$3"

    ./regex "$regex" "$input"
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$regex $input => $actual"
    else
        echo "$regex $input => $expected expected, but got $actual"
        exit 1
    fi
}

assert a a 0
assert a b 1
assert aa ab 1
assert a* ab 1

echo passed!!