CFLAGS=-std=c11 -g -static

regex:	regex.c

test:	regex
		./test.sh

clean:
	rm -f regex *.o *~ tmp*

# PHONYは疑似ターゲットと呼ばれ、ファイルが存在しないことを明示しコマンドとして使えるようにする
.PHONY:	test clean