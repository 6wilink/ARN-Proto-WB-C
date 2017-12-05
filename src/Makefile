# maintainer: Qige <qigezhao@gmail.com>
# updated on: 2017.11.08

APP=arn-wb

all: $(APP)

$(APP): *.c utils/*.c gws2/*.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -I. -o $@ $^ -L. -liwinfo -lnl-tiny -luci

clean:
	rm -f $(APP) src/*.o src/utils/*.o src/gws2/*.o
