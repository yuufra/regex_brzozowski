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
assert a* aa 0
assert a* ab 1
assert ab*c ac 0
assert ab*c ab 1
assert ab*c abc 0
assert ab*c abbc 0
assert "a|b" a 0
assert "a|b" b 0
assert "a|b" c 1
assert "ab*|b*a" ab 0
assert "(a)b" ab 0
assert "(ab)*b" b 0
assert "(ab)*b" abb 0
assert "(ab)*b" ababb 0
assert "(ab|cd)e" abe 0
assert "a+" a 0
assert "a+" aaa 0
assert "a+" ab 1
assert "ab?" a 0
assert "ab?" ab 0
assert "ab?" ac 1
assert "(ab+|cd?)*" abcabbcd 0
assert "." a 0
assert ".*" abc 0
assert "(a|.?b)+" c 1
assert "(ab*|.?d)+" addabdc 1

echo passed!!