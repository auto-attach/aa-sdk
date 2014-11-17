/* aa-sdk/common/common/aasdk_comm.c */

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

#include <stdlib.h>
#include <string.h>

#include "aasdk_comm.h"
#include "aasdk_osif_comm.h"
#include "aasdk_errno.h"

#include "aa_agent.h"


/*
 * global data for all interface layers
 */

/* output */
unsigned int        aasdk_output_flags;
aasdk_output_func_t aasdk_output_func;


/*
 * init functions
 */
int
aasdki_global_init(void)
{
    FA_AGT_init(NULL);
    faAgent_main_init();

    return AA_SDK_ENONE;
}


int
aasdki_global_ena(void)
{
    int fa_rc;
    int aa_rc;

    aasdkx_mutex_lock(faAgentMut);

    fa_rc = faAgentSetServiceStatus(FA_AGENT_STATE_ENABLED,
                                    FA_NV_SAVE,
                                    FA_DISTRIBUTE);

    aasdkx_mutex_unlock(faAgentMut);

    aa_rc = (fa_rc == 1) ? AA_SDK_ENONE : AA_SDK_EUNSPEC;

    return aa_rc;
}


int
aasdki_global_dis(void)
{
    int fa_rc; /* FA core status (1,0) */
    int aa_rc; /* AA SDK status AA_SDK_E* */

    aasdkx_mutex_lock(faAgentMut);

    fa_rc = faAgentSetServiceStatus(FA_AGENT_STATE_DISABLED,
                                    FA_NV_SAVE,
                                    FA_DISTRIBUTE);

    aasdkx_mutex_unlock(faAgentMut);

    aa_rc = (fa_rc == 1) ? AA_SDK_ENONE : AA_SDK_EUNSPEC;

    return aa_rc;
}



int
aasdki_glbl_stats_get(aasdk_stats_data_t *p_glbl_stats)
{
    if (p_glbl_stats == NULL)
    {
        return AA_SDK_EINVAL;
    }

    aasdkx_mutex_lock(faAgentMut);

    *p_glbl_stats = aaGlobalStats;
     p_glbl_stats->aasdk_port_id = -1;

    aasdkx_mutex_unlock(faAgentMut);

    return AA_SDK_ENONE;
}


int
aasdki_glbl_stats_clr(void)
{
    aasdkx_mutex_lock(faAgentMut);

    memset(&aaGlobalStats, 0, sizeof(aaGlobalStats));
    aaGlobalStats.aasdk_port_id = -1;

    aasdkx_mutex_unlock(faAgentMut);

    return AA_SDK_ENONE;
}

void
aasdki_output_init(void)
{
  aasdk_output_flags = 0;
  aasdk_output_func  = NULL;
}

int
aasdki_output_set(unsigned int         output_flags,
                  aasdk_output_func_t  output_func)
{
  aasdk_output_flags = output_flags;
  aasdk_output_func  = output_func;

  faAgentConfigData.faAgentDisplayInfoMsgs = 0;

  return AA_SDK_ENONE;
}


int
aasdk_output(char *output_string, unsigned int output_flag_num)
{
    int rc = 0;
    unsigned int output_flag;

    /* if there's an output function and the flag num is valid */

    if ((aasdk_output_func != NULL) &&
        (output_flag_num < (sizeof(unsigned int) * 8)))
        
    {
        /* if this flag is enabled */
        output_flag = 1 << output_flag_num;
        if (output_flag & aasdk_output_flags)
	{
	    /* then output the string */
            rc = (*aasdk_output_func)(output_string);
        }
    }

    return rc;
}


const char *
aasdku_errno_string(int errno)
{
    const char *perrno_string = NULL;

    switch(errno) {

    case AA_SDK_EUNSPEC:
        perrno_string = "EUNSPEC";
        break;

    case AA_SDK_ENONE:
        perrno_string = "ENONE";
        break;

    case AA_SDK_EIO:
        perrno_string = "EIO";
        break;

    case AA_SDK_EAGAIN:
        perrno_string = "EAGAIN";
        break;

    case AA_SDK_ENOMEM:
        perrno_string = "ENOMEM";
        break;

    case AA_SDK_EFAULT:
        perrno_string = "EFAULT";
        break;

    case AA_SDK_EBUSY:
        perrno_string = "EBUSY";
        break;

    case AA_SDK_ENODEV:
        perrno_string = "ENODEV";
        break;

    case AA_SDK_EINVAL:
        perrno_string = "EINVAL";
        break;

    case AA_SDK_ELOOP:
        perrno_string = "ELOOP";
        break;

    case AA_SDK_ETIME:
        perrno_string = "ETIME";
        break;

    case AA_SDK_EADDRINUSE:
        perrno_string = "EADDRINUSE";
        break;

    case AA_SDK_EADDRNOTAVAIL:
        perrno_string = "EADDRNOTAVAIL";
        break;

    case AA_SDK_ENOTSUP: /* not supported */
        perrno_string = "ENOTSUP";
        break;

    default:
        perrno_string = NULL;
        break;

    }

    return perrno_string;
}
