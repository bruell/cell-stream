/*
 * SPU programm for 9x9 convolution
 */
 
#include "program.h"
#include <vec_types.h>
#include <spu_intrinsics.h>
#include <libimage.h>
#include <conv_defs.h>

#define MASK_SIZE 81
#define CHUNK_INT_PER_LINE	(CHUNK_WIDTH / 4)

vec_int4		conv9_mask[MASK_SIZE];

void set_conv9_mask(s32_t* param) {
	u8_t		i;

    for (i = 0; i < MASK_SIZE; i++) {
    	conv9_mask[i] = _splat_int(param[i]);
    }
}

void chunk_handler(void) {
    u32_t	x, y, i;
    s32_t	value;
    data_t*	lines[9];
    data_t*	result;
	data_t* input	= prog_get_input_ptr();
	data_t* output	= prog_get_output_ptr();
	s32_t*	mask	= prog_get_param_ptr();
	
//	memcpy(output, input, CHUNK_SIZE);
//	return;

	lines[0] = (input + (CHUNK_WIDTH * 0));
	lines[1] = (input + (CHUNK_WIDTH * 1));
	lines[2] = (input + (CHUNK_WIDTH * 2));
	lines[3] = (input + (CHUNK_WIDTH * 3));
	lines[4] = (input + (CHUNK_WIDTH * 4));
	lines[5] = (input + (CHUNK_WIDTH * 5));
	lines[6] = (input + (CHUNK_WIDTH * 6));
	lines[7] = (input + (CHUNK_WIDTH * 7));
	lines[8] = (input + (CHUNK_WIDTH * 8));
	
	result = (output + (CHUNK_WIDTH * CHUNK_BORDER) + CHUNK_BORDER);
	
	for (y = 0; y < (CHUNK_HEIGHT - (2 * CHUNK_BORDER)); y++) {
		for (x = 0; x < (CHUNK_WIDTH - (2 * CHUNK_BORDER)); x++) {
			value = 0;
			for (i = 0; i < 9; i++) {
				value
					+= ( (s32_t)lines[0][x + i].value * mask[(9 * 0) + i] )
					 + ( (s32_t)lines[1][x + i].value * mask[(9 * 1) + i] )
					 + ( (s32_t)lines[2][x + i].value * mask[(9 * 2) + i] )
					 + ( (s32_t)lines[3][x + i].value * mask[(9 * 3) + i] )
					 + ( (s32_t)lines[4][x + i].value * mask[(9 * 4) + i] )
					 + ( (s32_t)lines[5][x + i].value * mask[(9 * 5) + i] )
					 + ( (s32_t)lines[6][x + i].value * mask[(9 * 6) + i] )
					 + ( (s32_t)lines[7][x + i].value * mask[(9 * 7) + i] )
					 + ( (s32_t)lines[8][x + i].value * mask[(9 * 8) + i] );
			}
			result[x].value = (u8_t)(value / 255);
		}
		
		
		lines[0] = lines[1];
		lines[1] = lines[2];
		lines[2] = lines[3];
		lines[3] = lines[4];
		lines[4] = lines[5];
		lines[5] = lines[6];
		lines[6] = lines[7];
		lines[7] = lines[8];
		lines[8] += CHUNK_WIDTH;
		
		result += CHUNK_WIDTH;
		
//		log_msg("conv9: line %d finished\n", y);
		
	}
	
	/*
	set_conv9_mask( prog_get_param_ptr() );
	
    // Calc
    lines[0] = (const unsigned int*)(input + (CHUNK_WIDTH * 0));
    lines[1] = (const unsigned int*)(input + (CHUNK_WIDTH * 1));
    lines[2] = (const unsigned int*)(input + (CHUNK_WIDTH * 2));
    lines[3] = (const unsigned int*)(input + (CHUNK_WIDTH * 3));
    lines[4] = (const unsigned int*)(input + (CHUNK_WIDTH * 4));
    lines[5] = (const unsigned int*)(input + (CHUNK_WIDTH * 5));
    lines[6] = (const unsigned int*)(input + (CHUNK_WIDTH * 6));
    lines[7] = (const unsigned int*)(input + (CHUNK_WIDTH * 7));
    lines[8] = (const unsigned int*)(input + (CHUNK_WIDTH * 8));

    out = (unsigned int*)(output + (CHUNK_WIDTH * CHUNK_BORDER));
		    
	conv9x9_4ub(lines, out,	conv9_mask, CHUNK_INT_PER_LINE, 1, 0);
    out += CHUNK_INT_PER_LINE;
	
	for (i = 0; i < (CHUNK_HEIGHT - (2 * CHUNK_BORDER)); i++) {
		lines[0] = lines[1];
		lines[1] = lines[2];
		lines[2] = lines[3];
		lines[3] = lines[4];
		lines[4] = lines[5];
		lines[5] = lines[6];
		lines[6] = lines[7];
		lines[7] = lines[8];
	
		lines[8] += CHUNK_INT_PER_LINE;
	
		conv9x9_4ub(lines, out, conv9_mask, CHUNK_INT_PER_LINE, 1, 0);
		out += CHUNK_INT_PER_LINE;
    }
    */
}

int main(u64_t spe_ctx ATTR_UNUSED, addr64_t cb_addr) {
	u32_t	i;
	s32_t	param[MASK_SIZE];

    // Set default mask
    for (i = 0; i < MASK_SIZE; i++) param[0] = 0;
    param[40] = 1;
    set_conv9_mask(param);
    
    prog_register_chunk_handler(&chunk_handler);
    
    prog_run(cb_addr);
    
    return 0;
}

