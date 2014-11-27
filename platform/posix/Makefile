# aa-sdk/platform/posix/Makefile
#
# Copyright (c) 2014, Avaya Inc.
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

# platform files

OBJS =  aasdk_lldpif_plat.o \
        aasdk_msgif_plat.o \
        aasdk_osif_plat.o \
        aasdk_time_plat.o 

CPPFLAGS = -I. -I../../common/common \
        -I../../transport/common \
        -DENABLE_AA \
        -DHAVE_LLDP \
        -I../../platform/stubs \
        -I../../lldp/src/daemon \
        -I../../lldp/src \
        -I../../lldp

clean:
	rm -f $(OBJS)

all: posix

posix: $(OBJS)

$(OBJS) : %.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

