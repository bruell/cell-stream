/**
 * Cell stream image
 */

#pragma once

#include "../../common.h"
#include "../../stat.hpp"
#include "chunk.hpp"

#include <string>
#include <iostream>
#include <limits>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::gil;
namespace fs = boost::filesystem;

namespace cell_stream {

class image : public chunk_source, public chunk_destination {

public:
	image() :
		chunk_source(), chunk_destination(),
		seq_id_(0), chunks_out_(0),
		chunk_border_(CHUNK_BORDER), chunk_width_(CHUNK_WIDTH), chunk_height_(CHUNK_HEIGHT) { }
	virtual ~image() { }

	void			in_file(const string& value)	{ in_file_ = value; }
	const string&	in_file(void)					{ return in_file_; }
	void			out_file(const string& value)	{ out_file_ = value; }
	const string&	out_file(void)					{ return out_file_; }
	void			sequence_id(const u8_t value)	{ seq_id_ = value; }
	u8_t			sequence_id(void)				{ return seq_id_; }
	stat&			statistics(void)				{ return stat_; }
	u32_t			chunks_out(void)				{ return chunks_out_; }

	void	set_chunk_border(const u32_t value)	{ chunk_border_ = value; }
	void	set_chunk_width(const u32_t value)	{ chunk_width_ = value; }
	void	set_chunk_height(const u32_t value)	{ chunk_height_ = value; }
	
	u32_t	image_width(void)	{ return in_image_.width(); }
	u32_t	image_height(void)	{ return in_image_.height(); }

	void	process_image(void);

	// chunk_destination
	void	chunk_received(const chunk_t*);
	
private:
	void	load_image(void);

	u32_t	calc_chunk_x(u32_t);
	u32_t	calc_chunk_y(u32_t);
	u32_t	calc_chunk_count(void);

	string	in_file_;
	string	out_file_;
	u8_t	seq_id_;
	stat	stat_;	
	u32_t	chunks_out_;

	u32_t	chunk_border_;
	u32_t	chunk_width_;
	u32_t	chunk_height_;
	
	gray8_image_t	in_image_;
	gray8_image_t	out_image_;
	
	boost::thread*	process_image_thread_;
	
};

class image_directory : public chunk_source, public chunk_destination {

public:
	image_directory() : _current_id(1) { }
	virtual ~image_directory() { }

	void			in_dir(const string& value)		{ _in_dir = value; } 
	const string&	in_dir(void)					{ return _in_dir; }
	void			out_dir(const string& value)	{ _out_dir = value; }
	const string&	out_dir(void)					{ return _out_dir; }
	void			sequence_id(const u8_t value)	{ _seq_id = value; }
	u8_t			sequence_id(void)				{ return _seq_id; }
	
	void	process_images(void);
	
	u64_t	avg_process_time(void);
	
private:
	typedef map<u32_t, image*> image_map_t;

	u32_t	get_id(void);
	void	chunk_generated(const u32_t, chunk_t*);

	// chunk_destination
	void	chunk_received(const chunk_t*);

	u32_t		_current_id;

	string		_in_dir;
	string		_out_dir;
	u8_t		_seq_id;
	
	image_map_t	_image_map;
	
};

}
