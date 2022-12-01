#include "shares.h"
#include "taskshare.h"

#ifndef _NETWORK_
#define _NETWORK_

void setup_wifi(void);
void task_webserver (void* p_params);

#endif // _NETWORK_