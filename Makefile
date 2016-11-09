CC=g++
CPPFLAGS= -Wall -g -O3
LDFLAGS=
LIBS=

PREFIX=/usr/bin

VCFDIVIDER_OBJS=vcfdivider.o main.o
VCFDIVIDER_HEADERS=vcfdivider.h
LIBCRE=crelib/libcre.a
LIBCRE_OBJS=crelib/*

all: vcfdivider

vcfdivider:$(VCFDIVIDER_OBJS) $(LIBCRE)
	$(LINK.cpp) $^ -o $@

$(VCFDIVIDER_OBJS): $(VCFDIVIDER_HEADERS)

$(LIBCRE):$(LIBCRE_OBJS)
	cd crelib && $(MAKE)

clean:
	rm *.o vcfdivider

install:all
	install -d $(PREFIX)
	install vcfdivider $(PREFIX)

remove:
	rm $(PREFIX)/vcfdivider
