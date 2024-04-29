/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APOLLOLAKE_RAMSTAGE_H_
#define _SOC_APOLLOLAKE_RAMSTAGE_H_

#include <fsp/api.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *silconfig);
#if CONFIG(RED_UNLOCK)
void red_unlock_payload(void);
#endif

#endif /* _SOC_APOLLOLAKE_RAMSTAGE_H_ */
