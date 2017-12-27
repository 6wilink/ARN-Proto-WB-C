# maintainer: Qige <qigezhao@gmail.com>
# updated on: 2017.11.08

ALL=arn-wb
EXTRA_CFLAGS=-l gws2 -l iwinfo -l nl-tiny

all: $(ALL)

$(ALL): *.c utils/*.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -I. -o $@ $^ -L.

clean:
	rm -f $(APP) src/*.o src/utils/*.o
