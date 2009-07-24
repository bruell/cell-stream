/**
 * SPU Controller
 */

#pragma once

#include "../css.h"
#include "../../stat.hpp"
#include "buffer.hpp"

#include <libspe2.h>
#include <libsync.h>
#include <libimage.h>
#include <conv9x9_4ub.h>

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace std;

namespace cell_stream {

class spu_controller {

public:
	spu_controller(cell_stream::buffer& buffer) :
		buffer_(buffer)
	{
		init();
	}
	virtual ~spu_controller() { }

	void	program_load(const u8_t, const string&);
	void	program_run(const u8_t, const u8_t);
	
	u8_t*	parameter_alloc(const u8_t, const size_t);
	void	parameter_free(const u8_t);
	u8_t*	parameter_data(const u8_t);
	size_t	parameter_size(const u8_t);
	
	void	sequence_add_entry(const u8_t, const u8_t, const u8_t);

	void	chunk_cache_values(chunk_t*);
	
	cell_stream::buffer&	buffer(void) { return buffer_; }	

private:
	typedef struct {
		control_block_t		control_block	ALIGN_128;
	    spe_context_ptr_t	context;
		u8_t				num;
	    u8_t				prog_id;
	    bool				stop;
	} spu_data_t;

	typedef vector< spu_data_t* >				spu_data_vector_t;
	typedef map< u8_t, spe_program_handle_t* >	spu_program_map_t;
	typedef pair< u8_t*, size_t	>				spu_parameter_t;
	typedef map< u8_t, spu_parameter_t >		spu_parameter_map_t;
	typedef pair< u8_t, u8_t >					spu_sequence_entry_t;
	typedef vector< spu_sequence_entry_t >		spu_sequence_t;
	typedef map< u8_t, spu_sequence_t >			spu_sequence_map_t;
	
	void	init(void);
	void	spu_manage(spu_data_t*);
	void	spu_run(const spu_data_t*);

	cell_stream::buffer&	buffer_;
	spu_data_vector_t		spu_data_;
	spu_program_map_t		spu_program_;
	spu_parameter_map_t		spu_parameter_;
	spu_sequence_map_t		spu_sequence_;
	
	boost::thread_group		work_group;

};	
	
}

/*

void spu_controller_init(void);
void spu_controller_load(u8_t, const char*);
void spu_controller_run(u8_t, u8_t);
void spu_controller_join(void);

void spu_controller_show_stat(void);

*/
