
#include "Image.hpp"

namespace cell_stream {

// image class

void image::process_image(void) {
	process_image_thread_ = new boost::thread(
		boost::bind(&image::load_image, this)	
	);
}

void image::load_image(void) {
	u32_t		i, x, y, width, height, chunk_count;
	chunk_t*	chunk;
	
	stat_.start();

	try {
		if ( boost::iends_with(in_file(), ".jpg") || boost::iends_with(in_file(), ".jpeg") ) {
			jpeg_read_and_convert_image(in_file(), in_image_);
		} else if ( boost::iends_with(in_file(), ".png") ) {
//			png_read_and_convert_image(in_file(), in_image_);			
		} else {
			cerr << "client: image load error: no handler for image file format" << endl;
			return;		
		}

		out_image_.recreate(in_image_.dimensions());
		cout << "client: image loaded ( " << in_image_.width() << " x " << in_image_.height() << " )" << endl;
	} catch (exception& e) {
		cerr << "client: image load error: " << e.what() << endl;
		return;
	}
	
	gray8_view_t				chunk_view;
	gray8_view_t::x_iterator	chunk_iter;
	
	chunk_count = calc_chunk_count();
	cout << "client: load " << chunk_count << " chunks" << endl;
	chunks_out_ = chunk_count;

	for (i = 0; i < chunk_count; i++) {
		chunk = new chunk_t();
		if ( chunk == NULL ) {
			cerr << "Cannot allocate memory for chunk (number " << i << ")" << endl;
		} else {
			chunk->x = calc_chunk_x(i);
			chunk->y = calc_chunk_y(i);
			chunk->seq_id = seq_id_;

			width	= min( image_width() - chunk->x, chunk_width_ );
			height	= min( image_height() - chunk->y, chunk_height_ );

//			cout << "client: load chunk (id " << chunk->id << " | " << width << " | " << height << ")" << endl;

			chunk_view = subimage_view(
				view(in_image_),
				chunk->x - chunk_border_,
				chunk->y - chunk_border_,
				width, height
			);

		    for (y = 0; y < height; y++) {
		    	chunk_iter = chunk_view.row_begin(y);
				for (x = 0; x < width; x++) {
					chunk->data[ (y * chunk_width_) + x ].value = (*chunk_iter++);
				}
		    }

			on_chunk_generated(chunk);
//			chunkReceived(chunk);

			delete chunk;
		}
		
//		boost::thread::yield();
	}
}

void image::chunk_received(const chunk_t* chunk) {
	u32_t	x, y, width, height, offset;

	width	= min( image_width() - chunk->x, chunk_width_ - (chunk_border_ * 2) );
	height	= min( image_height() - chunk->y, chunk_height_ - (chunk_border_ * 2) );

	gray8_view_t::x_iterator chunk_iter;
	gray8_view_t chunk_view = subimage_view(
		view(out_image_),
		chunk->x, chunk->y,
		width, height
	);

//	cout << "client: save chunk (id " << chunk->id << " | " << width << " | " << height << ")" << endl;

	offset = (chunk_border_ * chunk_width_) + chunk_border_;
    for (y = 0; y < height; y++) {
    	chunk_iter = chunk_view.row_begin(y);
		for (x = 0; x < width; x++) {
			(*chunk_iter++) = chunk->data[ offset + (y * chunk_width_) + x ].value;
		}
    }

	chunks_out_--;
	if (chunks_out_ == 0) {
		try {
			if ( boost::iends_with(out_file(), ".jpg") || boost::iends_with(out_file(), ".jpeg") ) {
				jpeg_write_view(out_file(), view(out_image_));	
			}
			
			cout << "client: image saved" << endl;

			stat_.stop();
		} catch (exception& e) {
			cerr << "client: image save error: " << e.what() << endl;
		}		
	}
}

u32_t image::calc_chunk_x(u32_t index) {
	return ( chunk_border_ +
		( chunk_width_ - (2 * chunk_border_) ) *
		( index % (u32_t)ceil( image_width() / (double)(chunk_width_ - (2 * chunk_border_)) ) )
	);
}

u32_t image::calc_chunk_y(u32_t index) {
	return ( chunk_border_ +
		( chunk_height_ - (2 * chunk_border_) ) *
		(u32_t)floor( index / ceil( image_width() / (double)(chunk_width_ - (2 * chunk_border_)) ) )
	);
}

u32_t image::calc_chunk_count(void) {
	return (
		(u32_t)ceil( image_width() / (double)(chunk_width_ - (2 * chunk_border_)) ) *
		(u32_t)ceil( image_height() / (double)(chunk_height_ - (2 * chunk_border_)) )
	);
}

// image_directory class

void image_directory::process_images(void) {
	fs::path	in_path( fs::initial_path<fs::path>() );
	fs::path	out_path( fs::initial_path<fs::path>() );
	u32_t		id;

	in_path		= fs::system_complete( fs::path(in_dir(), fs::native) );
	out_path	= fs::system_complete( fs::path(out_dir(), fs::native) );
	
	if ( fs::is_directory(in_path) ) {
		fs::directory_iterator end_iter;
		for (fs::directory_iterator dir_iter(in_path); dir_iter != end_iter; dir_iter++) {
			if ( !fs::is_regular(dir_iter->status()) ) continue;

			cout << dir_iter->path().file_string() << endl;				

			id = get_id();
			_image_map[id] = new image();
			_image_map[id]->in_file( (in_path / dir_iter->leaf()).file_string() );
			_image_map[id]->out_file( (out_path / dir_iter->leaf()).file_string() );
			
			_image_map[id]->signal_chunk_generated().connect(
				boost::bind( &image_directory::chunk_generated, this, id, _1 )
			);
				
			cout << "image_directory: process image (in " << _image_map[id]->in_file() << " / out " << _image_map[id]->out_file() << ")" << endl;
			_image_map[id]->process_image();
		}
	} else {
		cerr << "image_directory: error opening image directory " << in_dir() << endl;		
	}
}

u64_t image_directory::avg_process_time(void) {
	u64_t result = 0;
	
	if ( _image_map.size() > 0 ) {
		for (image_map_t::iterator i = _image_map.begin(); i != _image_map.end(); i++) {
			result += i->second->statistics().avg();
		}
		result /= _image_map.size();
	}
	
	return result;
}

u32_t image_directory::get_id(void) {
	u32_t result = _current_id;
	
	if ( _current_id < numeric_limits< u32_t >::max() ) {
		_current_id++;
	} else {
		_current_id = 0;
	}
	
	return result;
}

void image_directory::chunk_generated(const u32_t id, chunk_t* chunk) {
	chunk->id = id;
	on_chunk_generated(chunk);
}

void image_directory::chunk_received(const chunk_t* chunk) {
	if ( _image_map.find( chunk->id ) == _image_map.end() ) {
		cerr << "image_directory: cannot assign chunk id " << (int)chunk->id << " to an image" << endl;
	} else {
		_image_map[ chunk->id ]->chunk_received(chunk);
		
		if ( _image_map[ chunk->id ]->chunks_out() == 0 ) {
			delete _image_map[ chunk->id ];
			_image_map.erase( chunk->id );
		}
	}
}

}
