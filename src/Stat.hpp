/**
 * Statistics
 */
 
#pragma once

#include "../../common.h"
#include <iostream>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::posix_time;

namespace cell_stream {

class stat {
public:
	stat() { init(1); }
	virtual ~stat() { }
	
	void init(const u32_t size) {
		_active	= false;
		_index	= 0;
		_count	= 0;
		_duration.resize(size);
	}
	
	void now(void) {
		stop();
		start();
	}
	
	void start(void) {
		_active = true;
		_last = microsec_clock::local_time();
	}
	
	void stop(void) {
		if ( _active && (_duration.size() > 0) ) {
			_duration[_index] = microsec_clock::local_time() - _last;
			_index++;
			if ( _index >= _duration.size() ) _index = 0;
			if ( _count < _duration.size() ) _count++;
		}		
	}
	
	u64_t avg(void) {
		u64_t result = 0;
		
		if ( _count > 0 ) {
			for (u32_t i = 0; i < _count; i++) {
				result += (u64_t)_duration[i].total_microseconds();
			}
			result /= _count;
		}
		
		return result;		
	}
	
	u32_t count(void) {
		return _count;
	}
	
private:
	typedef vector< time_duration >	duration_t;

	bool		_active;
	
	ptime		_last;
	duration_t	_duration;
	u32_t		_index;
	u32_t		_count;

};

}
