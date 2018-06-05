#ifndef _ADMIN_PASSIVE_HANDLERS_H_
#define _ADMIN_PASSIVE_HANDLERS_H_

#include "../utils/selector/selector.h"

void adminPassiveAccept(struct selector_key * key);

void destroyAdminConnection(struct selector_key * key);

#endif