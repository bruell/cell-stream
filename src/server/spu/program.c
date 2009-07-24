
#include "program.h"

data_t	input[CHUNK_LENGTH]		ALIGN_128;
data_t	output[CHUNK_LENGTH]	ALIGN_128;
s32_t	param0[400]				ALIGN_128;
u16_t	size0;

void (*chunk_handler_func)(void) = NULL;

void prog_register_chunk_handler(void (*func)(void)) {
	chunk_handler_func = func;
}

data_t*	prog_get_input_ptr(void) {
	return input;
}

data_t*	prog_get_output_ptr(void) {
	return output;
}

s32_t*	prog_get_param_ptr(void) {
	return param0;
}

u16_t	prog_get_param_size(void) {
	return size0;
}

void prog_run(addr64_t cb_addr) {
	control_block_t	control_block		ALIGN_128;
	
	u32_t	tag			= 0;
    u8_t	stop		= 0;
    u32_t	in_msg		= 0;
    u32_t	out_msg		= MSG_DONE;

    // Reserve a tag for the DMA transfer
    if ( (tag = mfc_tag_reserve()) == MFC_TAG_INVALID ) {
		log_err("program: couldn't reserve a tag for dma transfer\n");
		return;
    }

    // Transfer control block
    mfc_get(&control_block, cb_addr, sizeof(control_block_t), tag, 0, 0);
    mfc_write_tag_mask(1 << tag);
    mfc_read_tag_status_all();
    
	while ( !stop ) {

		// Wait for the signal of the PPU that the calculation is ready
		while ( spu_stat_in_mbox() < 1 ) ;
		in_msg = spu_read_in_mbox();

		switch (in_msg) {
		case MSG_CHUNK:
		    // Transfer control block
			mfc_get(&control_block, cb_addr, sizeof(control_block_t), tag, 0, 0);
		    mfc_write_tag_mask(1 << tag);
			mfc_read_tag_status_all();
   			
   			// Transfer parameter
   			size0 = control_block.size0;
			mfc_get(param0, control_block.param0, size0, tag, 0, 0);
		    mfc_write_tag_mask(1 << tag);
			mfc_read_tag_status_all();	

			// Transfer chunk
		    mfc_get(input, control_block.data, CHUNK_SIZE, tag, 0, 0);
		    mfc_write_tag_mask(1 << tag);
		    mfc_read_tag_status_all();
	
			// Call worker function
			if ( chunk_handler_func ) chunk_handler_func();	
			
		    // Transfer chunk
		    mfc_put(output, control_block.data, CHUNK_SIZE, tag, 0, 0);
		    mfc_write_tag_mask(1 << tag);
		    mfc_read_tag_status_all();

		    break;
		
		case MSG_EXIT:
			stop = 1;		
			break;
			
		default:
			break;
		}

		// Send signal to PPU to indicate that the calculation has finished
		spu_write_out_mbox(out_msg);
		
	}
}
