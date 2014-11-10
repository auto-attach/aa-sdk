# aa-sdk/Makefile
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

OUTPUT = aasdk_engtest

OBJS = \
	apps/engtest/aasdk_engtest.o \
	common/agent/aa_port.o \
	common/agent/aa_agent.o \
	common/agent/aa_server.o \
	common/agent/aa_public.o \
	common/agent/aa_snmpagt.o \
	common/common/aasdk_comm.o \
	common/common/aasdk_elem_comm.o \
	common/common/aasdk_msg_comm.o \
	common/common/aasdk_port_comm.o \
	common/common/aasdk_time_comm.o \
	platform/posix/aasdk_msgif_plat.o \
	platform/posix/aasdk_osif_plat.o \
	platform/posix/aasdk_time_plat.o \
	platform/stubs/aasdk_lldpif_plat.o


LIBS = \
	-lpthread \
	-lrt


clean:
	rm -f $(OUTPUT)

posix:
	$(CC) -o $(OUTPUT) $(OBJS) $(LIBS)

