/**
 * SPU Program framework
 */
 
#pragma once

#include "../css.h"
#include <spu_mfcio.h>
#include <spu_intrinsics.h>
#include <stdio.h>
#include <vec_types.h>
#include <libimage.h>
#include <conv_defs.h>

void	prog_register_chunk_handler(void (*func)(void));

data_t*	prog_get_input_ptr(void);
data_t*	prog_get_output_ptr(void);
s32_t*	prog_get_param_ptr(void);
u16_t	prog_get_param_size(void);

void	prog_run(addr64_t);
