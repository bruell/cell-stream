/**
 * Buffer management
 */

#pragma once

#include "../css.h"
#include <malloc_align.h>
#include <free_align.h>

#include <iostream>
#include <vector>
#include <boost/thread.hpp>

using namespace std;

namespace cell_stream {

class buffer {

public:
	buffer() { }
	virtual ~buffer() { }

	void		init(const u32_t);
	size_t		size(void) const { return _chunk.size(); }
	
	void		step_chunk(chunk_t*, const chunk_state_t, const chunk_state_t);
	chunk_t*	find_chunk(const chunk_state_t, const chunk_state_t);
	chunk_t*	find_wait_chunk(const u8_t);
	
	u32_t		count(const chunk_state_t state) const { return _count[state]; }
	
private:
	typedef vector< chunk_t* > chunk_vector_t;

	chunk_vector_t	_chunk;
	
	u32_t			_index[E_CHUNK_LENGTH];
	u32_t			_count[E_CHUNK_LENGTH];
	boost::mutex	_mutex[E_CHUNK_LENGTH];

};	
	
}

/*

#define BUFFER_NO_CHUNK	2048

typedef struct {
	chunk_t			chunk[BUFFER_SIZE];
	u8_t			occupied[BUFFER_SIZE];

	u32_t			index[MAX_ID];
	u32_t			count[MAX_ID];
	pthread_mutex_t	mutex[MAX_ID];
} buffer_t;

void		buffer_init(void);
void		buffer_lock(u8_t);
void		buffer_unlock(u8_t);

chunk_t*	buffer_get_chunk(u32_t);
u32_t		buffer_get_chunk_id_by_state(u8_t);
u32_t		buffer_get_chunk_id_by_prog_id(u8_t);
u32_t		buffer_count_chunk(u8_t);

void		buffer_arrival(u32_t);
void		buffer_occupy(u32_t);
void		buffer_step(u32_t);
void		buffer_depart(u32_t);

*/
