#ifndef _ADMIN_READ_STATE_H_
#define _ADMIN_READ_STATE_H_

#include <sys/types.h>

#include "../../utils/selector/selector.h"

#define REQUEST_METHOD 0
#define REQUEST_LENGTH_DATA 1
#define REQUEST_DATA 2

#define AUTH_LENGHT_PWD 0
#define AUTH_PWD 1



unsigned adminRead(struct selector_key * key);

#endif
