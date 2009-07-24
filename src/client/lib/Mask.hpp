/**
 * Mask
 */
 
#pragma once

#include "../../common.h"
#include "program.hpp"

#include <string>
#include <iostream>
#include <ostream>
#include <fstream>

using namespace std;

namespace cell_stream {

class mask : public parameter {
public:
	mask() : mask_(NULL) { }
	virtual ~mask() { }
	
	void	load_mask(const string&);
	void	save_mask(const string&);
	
	void	set_mask_size(u16_t);
	u16_t	mask_size(void) const;
	
	void	set_value(u16_t, u16_t, s32_t);
	s32_t	value(u16_t, u16_t) const;
	
	// CSParameter
	
	virtual u8_t*	parameter_data(void) const	{ return (u8_t*)mask_; }
	virtual size_t	parameter_size(void) const	{ return ( size_ * size_ * sizeof(s32_t) ); }
	
private:
	s32_t*	mask_;
	u16_t	size_;
	
};

ostream& operator<<(ostream&, const mask&);
istream& operator>>(istream&, mask&);

}
