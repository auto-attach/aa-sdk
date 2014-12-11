/* aa-sdk/common/common/aasdk_mgmtif_comm.h */

/*
 * Copyright (c) 2014, Avaya Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef AASDK_MGMTIF_COMM_H
#define AASDK_MGMTIF_COMM_H

#include <stdint.h>  /* uint*_t  */
#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool */

#include "aasdk_comm.h"


/*
 * Configuration
 */


/* element types */

enum {
    AA_SDK_ELEM_NOT_SUPPORTED = 1,
    AA_SDK_ELEM_SERVER,
    AA_SDK_ELEM_HOST,
    AA_SDK_ELEM_CLIENT_UNTAGGED,
    AA_SDK_ELEM_CLIENT_TAGGED
};


int aasdki_elem_type_cfg(int elem_type);  /* AA_SDK_ELEM_* */
int aasdki_elem_mgmt_vlan_cfg(uint16_t mgmt_vlan);
int aasdki_glbl_rem_asgn_exp_cfg(int rem_asgn_exp);

int aasdki_port_create( aasdk_port_id_t port_id);
int aasdki_port_destroy(aasdk_port_id_t port_id);

int aasdki_port_aa_ena(aasdk_port_id_t port_id);
int aasdki_port_aa_dis(aasdk_port_id_t port_id);

int aasdki_port_sys_id_cfg(aasdk_port_id_t port_id,
                           uint8_t *sys_id);

int aasdki_port_auth_cfg(aasdk_port_id_t port_id,
                         uint8_t *key, size_t key_len);
int aasdki_port_auth_ena(aasdk_port_id_t port_id);
int aasdki_port_auth_dis(aasdk_port_id_t port_id);

int aasdki_port_asgn_add(aasdk_port_id_t port_id,
                         uint32_t isid, uint16_t vlan);
int aasdki_port_asgn_rem(aasdk_port_id_t port_id,
                         uint32_t isid, uint16_t vlan);


/*
 * status
 */

/* ISID/VLAN assignment origin */

enum {
  AA_SDK_ISID_VLAN_ORIG_LOCAL = 0,
  AA_SDK_ISID_VLAN_ORIG_REMOTE_PROXY = 1,
  AA_SDK_ISID_VLAN_ORIG_REMOTE_CLIENT = 2
};

/* ISDI/VLAN assignment status */

enum {
    AA_SDK_ISID_VLAN_ASGN_UNKNOWN = 0,
    AA_SDK_ISID_VLAN_ASGN_PENDING,
    AA_SDK_ISID_VLAN_ASGN_ACTIVE,
    AA_SDK_ISID_VLAN_ASGN_REJECT,

    AA_SDK_ASGN_REJECTION_RESOURCE_ERR,
    AA_SDK_ASGN_REJECTION_DUPLICATE_ERR,
    AA_SDK_ASGN_REJECTION_INVALID_VLAN,
    AA_SDK_ASGN_REJECTION_UNKNOWN_VLAN,
    AA_SDK_ASGN_REJECTION_RESOURCE_VLAN,
    AA_SDK_ASGN_REJECTION_APP_INTERACTION,
    AA_SDK_ASGN_REJECTION_PERR_ACCEPTANCE
};

#define AA_SDK_SYS_ID_LEN 10

typedef struct aasdk_glbl_status_s {
    bool     enabled;
    int      elem_type;
    uint16_t mgmt_vlan;
    int      rem_asgn_exp;
} aasdk_glbl_status_t;

typedef struct aasdk_port_status_s {
    /* locally configured */
    aasdk_port_id_t port_id;
    bool            enabled;
    uint8_t         loc_sys_id[AA_SDK_SYS_ID_LEN];
    /* received from remote */
    int             elem_type;
    uint16_t        mgmt_vlan;
    uint8_t         rem_sys_id[AA_SDK_SYS_ID_LEN];
} aasdk_port_status_t;

typedef struct aasdk_asgn_status_s {
    aasdk_port_id_t port_id;
    uint32_t        isid;
    uint16_t        vlan;
    int             origin;  /* AA_SDK_ISID_VLAN_ORIG_* */
    int             status;  /* AA_SDK_ISID_VLAN_ASGN_* */
} aasdk_asgn_status_t;


int aasdki_glbl_status_get(aasdk_glbl_status_t *p_glbl_sts);

int aasdki_port_status_get( aasdk_port_id_t port_id,
                            aasdk_port_status_t *pportsts);
int aasdki_ports_status_get(size_t  max_num_ports,
                            size_t *act_num_ports,
                            aasdk_port_status_t *pportsts);

/* single assignment on a single port */
int aasdki_asgn_status_get( aasdk_port_id_t port_id,
                            uint32_t isid,
                            uint16_t vlan,
                            aasdk_asgn_status_t *pasgnsts);
/* all assignments on a single port */
int aasdki_port_asgns_status_get(aasdk_port_id_t port_id,
                                 size_t  max_num_asgns,
                                 size_t *act_num_asgns,
                                 aasdk_asgn_status_t *pasgnsts);
/* all assignments on all ports */
int aasdki_asgns_status_get(size_t  max_num_asgns,
                            size_t *act_num_asgns,
                            aasdk_asgn_status_t *pasgnsts);


/*
 * statistics
 */

typedef struct aasdk_stats_data_s {
    aasdk_port_id_t aasdk_port_id;  /* -1 for global */
    uint32_t        aasdk_disc_received;
    uint32_t        aasdk_asgn_received;
    uint32_t        aasdk_asgn_accepted;
    uint32_t        aasdk_asgn_rejected;
    uint32_t        aasdk_asgn_expired;
    uint32_t        aasdk_auth_failed;
} aasdk_stats_data_t;

int aasdki_glbl_stats_get(aasdk_stats_data_t *p_stats);

int aasdki_glbl_stats_clr(void);

int aasdki_port_stats_get(aasdk_port_id_t    port_id,
                          aasdk_stats_data_t *p_stats);

int aasdki_port_stats_clr(aasdk_port_id_t    port_id);



/*
 * events
 */

typedef int (*aasdk_event_func_t)(unsigned int, void *);

#define AA_SDK_EVENT_SERVER_DISCOVERED ( 1 << 0)
#define AA_SDK_EVENT_SERVER_LOST       ( 1 << 1)
#define AA_SDK_EVENT_PROXY_ENABLED     ( 1 << 2)
#define AA_SDK_EVENT_PROXY_DISABLED    ( 1 << 3)

/* < 0 error   - AA_SDK_E*
 * > 0 success - callback ID
 */
int
aasdk_event_register(unsigned int events,
                     aasdk_event_func_t event_handler,
                     void *context);

void
aasdk_event_unregister(int callback_id);


#endif /* ndef AASDK_MGMTIF_COMM_H */
