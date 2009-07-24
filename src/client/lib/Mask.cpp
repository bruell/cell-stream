
#include "Mask.hpp"

namespace cell_stream {

void mask::load_mask(const string& filename) {
	ifstream file(filename.c_str(), ios::in);
	if ( file.good() ) { 
		file >> *this;
	}
	file.close();	
}

void mask::save_mask(const string& filename) {
	ofstream file(filename.c_str(), ios::trunc);
	if ( file.good() ) {
		file << *this;
	}
	file.close();
}

void mask::set_mask_size(u16_t value) {
	size_ = value;

	mask_ = new s32_t[ size_ * size_ ];
	for (u16_t i = 0; i < (size_ * size_); i++) mask_[i] = 0;
}

u16_t mask::mask_size(void) const {
	return size_;
}

void mask::set_value(u16_t x, u16_t y, s32_t value) {
	if ( (x < size_) && (y < size_) ) {
		mask_[ x + (y * size_) ] = value; // htonl(value);
	}
}

s32_t mask::value(u16_t x, u16_t y) const {
	if ( (x < size_) && (y < size_) ) {
		return mask_[ x + (y * size_) ]; // ntohl(mask[ x + (y * size) ]);
	}
	
	return 0;
}

ostream& operator<<(ostream& stream, const mask& mask) {
	stream << "Size: " << mask.mask_size() << endl;
	
	for (u16_t y = 0; y < mask.mask_size(); y++) {
		stream << "[ "; 
		for (u16_t x = 0; x < mask.mask_size(); x++) {
			stream.width(5);
			stream << right << mask.value(x, y);
		}
		stream << " ]" << endl;
	}
		
	return stream;
}

istream& operator>>(istream& stream, mask& mask) {
	u16_t size;
	s32_t value;
	string s;
	
	stream >> skipws >> s >> skipws >> size;
	if ( s == "Size:" ) {
		mask.set_mask_size(size);		
	}

	for (u16_t y = 0; y < mask.mask_size(); y++) {
		stream >> skipws >> s;
		if ( s == "[" ) {
			for (u16_t x = 0; x < mask.mask_size(); x++) {
				stream >> skipws >> value;
				mask.set_value(x, y, value);
			}
			stream >> skipws >> s;
		}
	}

	return stream;
}

}
