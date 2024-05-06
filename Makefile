CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

regex:	$(OBJS)
		$(CC) -o regex $(OBJS) $(LDFLAGS)

$(OBJS): regex.h

test:	regex
		./test.sh

clean:
	rm -f regex *.o *~ tmp*

# PHONYは疑似ターゲットと呼ばれ、ファイルが存在しないことを明示しコマンドとして使えるようにする
.PHONY:	test clean