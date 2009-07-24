
#include "spu_controller.hpp"

namespace cell_stream {

void spu_controller::program_load(const u8_t prog_id, const string& filename) {
	// close spu program, that maybe opened before
	if ( spu_program_[prog_id] ) {
		spe_image_close(spu_program_[prog_id]);		
	}
	
	// open spu program image file
	spu_program_[prog_id] = spe_image_open( filename.c_str() );
	if ( spu_program_[prog_id] == NULL ) {
		cerr << "spu_controller: error while open spu image file " << filename << endl;
		return;
	}
	
	cout << "spu_controller: spu program " << filename << " loaded succesfully (id " << (int)prog_id << ")" << endl;
}

void spu_controller::program_run(const u8_t spu_id, const u8_t prog_id) {
	if ( spu_id >= spu_data_.size() ) {
		cerr << "spu_controller: spu number " << (int)spu_id << " dosen't exists" << endl;
		return;
	}
	
	if ( spu_program_[prog_id] == NULL ) {
		cerr << "spu_controller: no program for id " << (int)prog_id << "%d loaded" << endl;
		return;
	}

	// load the SPE program into the SPE context
	if ( spe_program_load(spu_data_[spu_id]->context, spu_program_[prog_id]) != 0 ) {
		cerr << "spu_controller: couldn't load spu program (id " << (int)prog_id << ")" << endl;
		return;
	}

	spu_data_[spu_id]->prog_id = prog_id;

	work_group.create_thread(
		boost::bind( &spu_controller::spu_manage, this, spu_data_[spu_id] )
	);
	
	cout << "spu_controller: spu program " << (int)prog_id << " started succesfully on spu " << (int)spu_id << endl;
}

u8_t* spu_controller::parameter_alloc(const u8_t param_id, const size_t size) {
	u8_t* result = NULL;
	
	if ( spu_parameter_.find(param_id) != spu_parameter_.end() ) {
		parameter_free(param_id);
	}
	
	size_t new_size = size + (128 - (size % 128));
	
	result = (u8_t*)_malloc_align(new_size, 7);
	if ( result ) {
		spu_parameter_[param_id].first = result;
		spu_parameter_[param_id].second = new_size;
	} else {
		cerr << "spu_parameter: couldn't allocate memory for parameter (id " << (int)param_id << ")" << endl;		
	}
	
	return result;
}

void spu_controller::parameter_free(const u8_t param_id) {
	if ( spu_parameter_.find(param_id) != spu_parameter_.end() ) {
		_free_align(spu_parameter_[param_id].first);
		
		spu_parameter_.erase(param_id);
	}
}

u8_t* spu_controller::parameter_data(const u8_t param_id) {
	if ( spu_parameter_.find(param_id) != spu_parameter_.end() ) {
		return spu_parameter_[param_id].first;
	} else {
		return NULL;
	}
}

size_t spu_controller::parameter_size(const u8_t param_id) {
	if ( spu_parameter_.find(param_id) != spu_parameter_.end() ) {
		return spu_parameter_[param_id].second;
	} else {
		return 0;
	}	
}

void spu_controller::sequence_add_entry(const u8_t seq_id, const u8_t prog_id, const u8_t param_id) {
	spu_sequence_[seq_id].push_back( spu_sequence_entry_t(prog_id, param_id) );
}

void spu_controller::chunk_cache_values(chunk_t* chunk) {
	if ( spu_sequence_.find( chunk->seq_id ) == spu_sequence_.end() ) return;
	if ( spu_sequence_[chunk->seq_id].size() >= chunk->seq_index ) return; 
		
	chunk->prog_id	= spu_sequence_[ chunk->seq_id ][ chunk->seq_index ].first;
	chunk->param_id	= spu_sequence_[ chunk->seq_id ][ chunk->seq_index ].second;
}

void spu_controller::init(void) {
	u8_t i;
	
	// Check SPUs
	
	cout << "spu_controller: count spes in system: " << (int)spe_cpu_info_get(SPE_COUNT_PHYSICAL_SPES, 0) << endl;
	cout << "spu_controller: count usable spes:    " << (int)spe_cpu_info_get(SPE_COUNT_USABLE_SPES, 0) << endl;

	spu_data_.resize( spe_cpu_info_get(SPE_COUNT_USABLE_SPES, 0) );
	
	// Create SPU context and load SPU program

    for (i = 0; i < spu_data_.size(); i++) {
    	spu_data_[i]		= (spu_data_t*)_malloc_align( sizeof(spu_data_t), 7 );
		spu_data_[i]->num	= i;
		spu_data_[i]->stop	= false;

		// create the SPE context
		if ((spu_data_[i]->context = spe_context_create(0, NULL)) == NULL) {
			cerr << "spu_controller: error while creating spu context number " << i << endl;
		}
	}
	
	// Set all program handles to NULL
	spu_program_.clear();
}

void spu_controller::spu_manage(spu_data_t* spu_data) {
	chunk_t*						chunk;
	spu_sequence_map_t::iterator	seq;
	u8_t*							param;
	u32_t							out_msg, in_msg;
	control_block_t*				cb = &spu_data->control_block;

	boost::thread runner_thread(
		boost::bind(&spu_controller::spu_run, this, spu_data)
	);

	in_msg = MSG_CHUNK;
	
	while ( !spu_data->stop ) {

//		boost::this_thread::yield();

//		cout << "spu_controller: search for prog id " << (int)spu_data->prog_id << endl;

		chunk = buffer_.find_wait_chunk( spu_data->prog_id );	
		if ( chunk == NULL ) continue;

//		cout << "spu_controller: spu mange " << (int)spu_data->num << " found a chunk" << endl;

		seq = spu_sequence_.find( chunk->seq_id );
		if ( seq == spu_sequence_.end() ) continue;

		param = parameter_data( chunk->param_id );
		if ( param == NULL ) continue;
			
		cb->data	= (addr64_t)(u32_t)chunk->data;
		cb->param0	= (addr64_t)(u32_t)param;
		cb->size0	= (u16_t)parameter_size( chunk->param_id );

		// Send message to SPU to start calculation
		spe_in_mbox_write(spu_data->context, &in_msg, 1, SPE_MBOX_ALL_BLOCKING);

//		cout << "spu_controller: chunk for spu " << (int)spu_data->num << " param " << (int)chunk->param_id << endl;

//		boost::this_thread::sleep(1);
//		boost::this_thread::yield();

		// Wait for the message of the SPU that the calculation has finished
		out_msg = MSG_NONE;
		while ( out_msg != MSG_DONE ) {
			while ( spe_out_mbox_status(spu_data->context) < 0 ) ;
			spe_out_mbox_read(spu_data->context, &out_msg, 1);
		}
		
		chunk->seq_index++;
		if ( chunk->seq_index < seq->second.size() ) {
			chunk_cache_values( chunk );
			buffer_.step_chunk( chunk, E_CHUNK_WORKING, E_CHUNK_WAIT );
		} else {
			buffer_.step_chunk( chunk, E_CHUNK_WORKING, E_CHUNK_READY );
		}
		
	}
		
	// Send signal to SPU to end execution
	in_msg = MSG_EXIT;
	spe_in_mbox_write(spu_data->context, &in_msg, 1, SPE_MBOX_ANY_NONBLOCKING);
	
	runner_thread.join();
}

void spu_controller::spu_run(const spu_data_t* spu_data) {
	unsigned int entry = SPE_DEFAULT_ENTRY;

	// Run SPU program
    if ( spe_context_run(spu_data->context, &entry, 0, (void*)&spu_data->control_block, NULL, NULL) < 0 ) {
    	cerr << "spu_controller: error while running spu " << (int)spu_data->num << endl;
    }	
}

}
