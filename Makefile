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
	apps/lldp/aasdk_lldp.o \
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
	common/common/aasdk_lldpif_comm.o \
	platform/posix/aasdk_msgif_plat.o \
	platform/posix/aasdk_osif_plat.o \
	platform/posix/aasdk_time_plat.o \
	platform/posix/aasdk_lldpif_plat.o \
	platform/posix/aasdk_trace_plat.o \
	lldp/src/compat/strlcpy.o \
	lldp/src/daemon/liblldpd_la-frame.o \
	lldp/src/daemon/liblldpd_la-lldp.o \
	lldp/src/daemon/liblldpd_la-lldpd.o \
	lldp/src/lldpd-structs.o \
	lldp/src/log.o \
	transport/common/aatrans_trace_comm.o \
        transport/common/aatrans_auth_comm.o \
	transport/common/aatrans_elem_comm.o \
	transport/common/aatrans_packet_comm.o \
	transport/common/aatrans_port_comm.o \
	transport/common/aatrans_time_comm.o \
	transport/common/aatrans_comm.o 


LIBS = \
	-lpthread \
	-lrt


clean:
	rm -f $(OUTPUT)

posix:
	$(CC) -o $(OUTPUT) $(OBJS) $(LIBS)

