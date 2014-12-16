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

#include <stdint.h>
#include <stdbool.h>
#include "sys/queue.h"
#include "lldp-const.h"
#include "aasdk_comm.h"
#include "aasdk_errno.h"
#include "aatrans_comm.h"
#include "aatrans_port_comm.h"
#include "aatrans_trace_comm.h"
#include "aatrans_time_comm.h"

#define AASDK_TRC_MOD_ID aasdk_mod_aatrans_common

/* lldp global config */
struct lldpd *lldp_cfg = NULL;
lldp_cfg_ext_t cfg_ext;

#define MAX_ETHER_ADDR_LEN_IN_BYTE	6
#define MAX_ETHER_ADDR_LEN_IN_WORD	3


/*
 * initialize transport protocol
 */
int
aatransi_global_init(aasdk_transport_config_t *config)
{
    struct lldpd_chassis *lchassis;

    aasdk_log("Initializing the transport module");

    /* initialize extended cfg */
    memset(&cfg_ext, 0, sizeof(cfg_ext));
    cfg_ext.send = config->send;
    cfg_ext.aa_element_type = LLDP_TLV_AA_ELEM_TYPE_SERVER; /* default, will get updated by AA */
    cfg_ext.aa_element_mgmt_vlan = 0;
    /* keep this list sorted based on isid */
//    hmap_init(&cfg_ext.mappings_by_isid);

    /* allocate the global lldpd cfg */
    if ((cfg = (struct lldpd *) calloc(1, sizeof(struct lldpd))) == NULL) {
        aasdk_log("Error: failed to allocate memory for lldpd_cfg");
        return AA_SDK_ENOMEM;
    }

    cfg->g_config.c_paused = config->paused;
    cfg->g_config.c_receiveonly = config->receiveonly;
    cfg->g_config.c_tx_interval = config->tx_interval;
    cfg->g_config.c_tx_hold = config->tx_hold;
    cfg->g_config.c_max_neighbors = config->max_neighbors;
    cfg->g_config.c_advertise_version = config->advertise_version;
    lldpd_assign_cfg_to_protocols(cfg);
    aatransi_time_interval_set(config->tx_interval);

// The following need not be exposed, keep them here for now
//    cfg->g_config.c_mgmt_pattern = mgmtp;
//    cfg->g_config.c_cid_pattern = cidp;
//    cfg->g_config.c_iface_pattern = interfaces;
//    cfg->g_snmp = snmp; /* used for snmp */
//    cfg->g_snmp_agentx = agentx; /* used for snmp */
//    cfg->g_lsb_release = lsb_release; /* get OS release */
//    cfg->g_config.c_description = descr_override;
//    cfg->g_config.c_platform = platform_override; /* CDP */
//    cfg->g_config.c_bond_slave_src_mac_type = config->bond_slave_src_mac_type;
//    cfg->g_config.c_enable_fast_start = enable_fast_start; /* used for MED */
//    cfg->g_config.c_tx_fast_init = LLDPD_FAST_INIT; /* used for MED */
//    cfg->g_config.c_tx_fast_interval = LLDPD_FAST_TX_INTERVAL; /* used for MED */
//    cfg->g_config.c_smart = 0; /* Turning this on causes seg fault in lldpd_hide_all */

    if ((lchassis = (struct lldpd_chassis*) 
         calloc(1, sizeof(struct lldpd_chassis))) == NULL) {
        aasdk_log("Error: failed to allocate memory for lldpd_chassis");
        return AA_SDK_ENOMEM;
    }

    lchassis->c_id_subtype = config->chassis_id_subtype;
    if ((lchassis->c_id = (char *)
         calloc(1, config->chassis_id_len)) == NULL) {
        aasdk_log("Error: failed to allocate memory for chassis_id");
        return AA_SDK_ENOMEM;
    }
    memcpy(lchassis->c_id, config->chassis_id, config->chassis_id_len);
    lchassis->c_id_len = config->chassis_id_len;

    if (!(lchassis->c_name = strdup(config->chassis_name))) {
        aasdk_log("Error: failed to copy string chassis_name");
        return AA_SDK_ENOMEM;
    }

    if (!(lchassis->c_descr = strdup(config->chassis_descr))) {
        aasdk_log("Error: failed to copy string chassis_descr");
        return AA_SDK_ENOMEM;
    }

    lchassis->c_cap_available = config->chassis_cap_available;
    lchassis->c_cap_enabled = config->chassis_cap_enabled;
    lchassis->c_ttl = cfg->g_config.c_tx_interval * cfg->g_config.c_tx_hold;

    TAILQ_INIT(&lchassis->c_mgmt);
    TAILQ_INIT(&cfg->g_chassis);
    TAILQ_INSERT_TAIL(&cfg->g_chassis, lchassis, c_entries);
    TAILQ_INIT(&cfg->g_hardware);

    return AA_SDK_ENONE;
}

/*
 * configure transport protocol
 */
int
aatransi_global_configure(void) /*struct lldp *lldp, const struct lldp_settings *settings)*/
{
#if 0
    /* tx/rx/txrx
     * tx interval
     * hold interval (how is this implemented in this context?)
     */
    if (settings->tx_interval_ms) {
        timer_set_expired(&lldp->tx_timer);
        timer_set_duration(&lldp->tx_timer, 1000 /*settings->tx_interval_ms*/);
        lldp->lldpd->g_config.c_tx_interval = settings->tx_interval_ms;
    }
#endif

    return AA_SDK_ENONE;
}

int
aatransi_tx_interval_set(uint32_t value)
{
    aasdk_trace(aa_verbose, "value=%d", value);

    if (value != cfg->g_config.c_tx_interval)
    {
        struct lldpd_chassis *lchassis = LOCAL_CHASSIS(cfg);

        cfg->g_config.c_tx_interval = value;
        lchassis->c_ttl = cfg->g_config.c_tx_interval * cfg->g_config.c_tx_hold;
        aatransi_time_interval_set(value);
    }

    return AA_SDK_ENONE;
}

int
aatransi_tx_interval_get(uint32_t *ret_val)
{
    if (ret_val == NULL) {
        aasdk_log("Error: null parameter");
        return AA_SDK_EINVAL;
    }

    *ret_val = cfg->g_config.c_tx_interval;

    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_tx_hold_multiplier_set(uint32_t value)
{
    aasdk_trace(aa_verbose, "value=%d", value);

    if (value != cfg->g_config.c_tx_hold)
    {
        struct lldpd_chassis *lchassis = LOCAL_CHASSIS(cfg);

        cfg->g_config.c_tx_hold = value;
        lchassis->c_ttl = cfg->g_config.c_tx_interval * cfg->g_config.c_tx_hold;
    }
    return AA_SDK_ENONE;
}

int
aatransi_tx_hold_multiplier_get(uint32_t *ret_val)
{
    if (ret_val == NULL) {
        aasdk_log("Error: null parameter");
        return AA_SDK_EINVAL;
    }

    *ret_val = cfg->g_config.c_tx_hold;

    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_reinit_delay_set(uint32_t value)
{
    /* do nothing: value is hardcoded in lldpd, change later */

    aasdk_trace(aa_verbose, "value=%d - to be implemented", value);

    return AA_SDK_ENONE;
}

int
aatransi_reinit_delay_get(uint32_t *ret_val)
{
    if (ret_val == NULL) {
        aasdk_log("Error: null parameter");
        return AA_SDK_EINVAL;
    }

    /* return hardcoded value in lldpd */
    *ret_val = 1;

    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_tx_delay_set(uint32_t value)
{
    /* do nothing: value is hardcoded in lldpd, change later */

    aasdk_trace(aa_verbose, "value=%d - to be implemented", value);

    return AA_SDK_ENONE;
}

int
aatransi_tx_delay_get(uint32_t *ret_val)
{
    if (ret_val == NULL) {
        aasdk_log("Error: null parameter");
        return AA_SDK_EINVAL;
    }

    /* return hardcoded value in lldpd */
    *ret_val = LLDPD_TX_MSGDELAY;

    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_notification_interval_set(uint32_t value)
{
    /* do nothing: value is hardcoded in lldpd, change later */

    aasdk_trace(aa_verbose, "value=%d - to be implemented", value);

    return AA_SDK_ENONE;
}

int
aatransi_notification_interval_get(uint32_t *ret_val)
{
    if (ret_val == NULL) {
        aasdk_log("Error: null parameter");
        return AA_SDK_EINVAL;
    }

    /* return hardcoded value in lldpd */
    *ret_val = 5;

    aasdk_trace(aa_verbose, "ret_val=%d", *ret_val);

    return AA_SDK_ENONE;
}

int
aatransi_global_stats_get(aasdk_transport_global_stats_t *stats)
{
    struct lldpd_hardware *hardware;

    if (stats == NULL) {
        aasdk_log("Error: null parameter");
        return AA_SDK_EINVAL;
    }

    aasdk_trace(aa_verbose, "getting global stats");

    TAILQ_FOREACH(hardware, &cfg->g_hardware, h_entries) {
        stats->inserts += hardware->h_insert_cnt;
        stats->ageouts += hardware->h_ageout_cnt;
        stats->deletes += hardware->h_delete_cnt;
        stats->drops += hardware->h_drop_cnt;
    }
    return AA_SDK_ENONE;
}

int
aatransi_chassis_macaddr_set(uint8_t *macaddr)
{
    aasdk_trace(aa_verbose, "to be implemented");

    return AA_SDK_ENONE;
}

int
aatransi_mtu_set(uint32_t mtu)
{
    aasdk_trace(aa_verbose, "mtu=%d - to be implemented", mtu);

    return AA_SDK_ENONE;
}

void
aatransi_print_lldp_and_aa_stats(print_func_t print_func, void *arg)
{
    if(!print_func || !arg) {
        return;
    }

    (print_func)(arg, "Statistics:\n");

    if (cfg) {
        struct lldpd_hardware *hardware;

        TAILQ_FOREACH (hardware, &cfg->g_hardware, h_entries) {
            (print_func)(arg, "\ttx cnt: %llu\n", (long long unsigned int) hardware->h_tx_cnt);
            (print_func)(arg, "\trx cnt: %llu\n",  (long long unsigned int) hardware->h_rx_cnt);
            (print_func)(arg, "\trx discarded cnt: %llu\n", (long long unsigned int) hardware->h_rx_discarded_cnt);
            (print_func)(arg, "\trx unrecognized cnt: %llu\n", (long long unsigned int) hardware->h_rx_unrecognized_cnt);
            (print_func)(arg, "\tageout cnt: %llu\n", (long long unsigned int) hardware->h_ageout_cnt);
            (print_func)(arg, "\tinsert cnt: %llu\n", (long long unsigned int) hardware->h_insert_cnt);
            (print_func)(arg, "\tdelete cnt: %llu\n", (long long unsigned int) hardware->h_delete_cnt);
            (print_func)(arg, "\tdrop cnt: %llu\n", (long long unsigned int) hardware->h_drop_cnt);
        }
    }
}


