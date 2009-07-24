/*
 * Common
 */

#pragma once
 
#include <stdlib.h>
#include <stdio.h>
#include <vec_types.h>
#include <libsync.h>

#include "../common.h"

#define log_msg(args...)	fprintf(stdout, args); fflush(stdout)
#define log_warn(args...)	fprintf(stdout, args); fflush(stdout)
#define log_err(args...)	fprintf(stderr, args); fflush(stderr)

// #define SPE_COUNT			6

#define ALIGN_16			__attribute__ ((aligned (16)))
#define ALIGN_128			__attribute__ ((aligned (128)))
#define ATTR_UNUSED			__attribute__ ((unused))

typedef unsigned long long	addr64_t;

#define MSG_NONE			0
#define MSG_CHUNK			1
#define MSG_PARAM			2
#define MSG_DONE			3
#define MSG_EXIT			4

typedef struct {
    addr64_t		data;
	addr64_t		param0;
	u16_t			size0;
	u8_t			padding[106];
} control_block_t;

