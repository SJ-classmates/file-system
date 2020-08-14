obj-c = $(wildcard ./src/*.c)
obj-o = $(patsubst %.c, %.o, $(obj-c))

CFLAGS = -I ./include
CC     = gcc
target = main

$(target):$(obj-o)
	$(CC) $^ -o $@ $(CFLAGS)
	
$(obj-o):%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
.PHONY:clean
	
clean:
	rm $(obj-o) $(target)