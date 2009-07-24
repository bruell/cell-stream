/**
 * Protocol defines
 */

#pragma once

#include "common.h"

// Types

typedef enum {
	E_CMD_NONE			= 0x00,
	E_CMD_STATUS		= 0x01,
	E_CMD_LOAD			= 0x02,
	E_CMD_RUN			= 0x03,
	E_CMD_PARAMETER		= 0x04,
	E_CMD_SEQUENCE		= 0x05,
	E_CMD_CHUNK			= 0x06,
	E_CMD_SHUTDOWN		= 0x07,
	E_CMD_CLOSE			= 0x08
} command_t;

