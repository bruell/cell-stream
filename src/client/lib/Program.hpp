/**
 * Parameter
 */
 
#pragma once

#include "../../common.h"
#include <vector>
#include <utility>

using namespace std;

namespace cell_stream {

class parameter {
public:
	parameter() { }
	virtual ~parameter() { }
	
	virtual u8_t*	parameter_data(void)	const	= 0;
	virtual size_t	parameter_size(void)	const	= 0;

private:
	
};

typedef pair< u8_t, u8_t >			sequence_entry_t;
typedef vector< sequence_entry_t >	sequence_t;

}
