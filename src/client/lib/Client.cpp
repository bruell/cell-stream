
#include "Client.hpp"

namespace cell_stream {

// Functions

void client::connect_chunk_source(chunk_source& cs) {
	cs.signal_chunk_generated().disconnect_all_slots();
	cs.signal_chunk_generated().connect(
		boost::bind( (boost::function<void (client*, chunk_t*)>)&client::send_chunk, this, _1 )
	); 
}

void client::connect_chunk_destination(chunk_destination& cd) {
	on_chunk_received_.disconnect_all_slots();
	on_chunk_received_.connect(
		boost::bind( (boost::function<void (chunk_destination*, chunk_t*)>)&chunk_destination::chunk_received, &cd, _1 )
	);
}

void client::clear_statistics(void) {
	recv_stat_.init(50);
	send_stat_.init(50);
}

void client::show_statistics(void) {
	cout << "Statistics:" << endl;
	
	cout << "\tAvg recv duration: ";
	cout.width(10);
	cout << right << recv_stat_.avg() << " us ";
	if ( recv_stat_.avg() > 0 ) {
		cout.width(10);
		cout << right << (sizeof(chunk_t) / (recv_stat_.avg() / 1000000.0)) << " bytes/sec ";
	}
	cout << "(" << recv_stat_.count() << " chunks)" << endl;
	 
	cout << "\tAvg send duration: ";
	cout.width(10);
	cout << right << send_stat_.avg() << " us ";
	if ( send_stat_.avg() > 0 ) {
		cout.width(10);
		cout << right << (sizeof(chunk_t) / (send_stat_.avg() / 1000000.0)) << " bytes/sec ";
	}
	cout << "(" << send_stat_.count() << " chunks)" << endl; 
}

const status_t&	client::server_status(void) {
	while ( !server_status_received_ ) boost::thread::yield();	
	return server_status_;
}

void client::connect(const string& host, int port) {
	tcp::resolver::query query(host, "6120");
	tcp::resolver::iterator endpoint_iter = resolver_.resolve(query);
	tcp::resolver::iterator end;
	
	boost::system::error_code error = boost::asio::error::host_not_found;
	while (error && endpoint_iter != end) {
		socket_.close();
		socket_.connect(*endpoint_iter++, error);		
	}
	if (error) throw boost::system::system_error(error);
	
	connected_ = true;
	
	async_read( sizeof(u8_t) );

	io_thread_ = new boost::thread(
		boost::bind(&boost::asio::io_service::run, &io_service_)
	);
}

void client::disconnect(void) {
	if ( !connected_ ) return;

	connected_ = false;

	io_service_.stop();
	io_thread_->join();
	delete io_thread_;
}

void client::send_status_request(void) {
	if ( !connected_ ) return;
	
	boost::mutex::scoped_lock lock(output_mutex_);

	server_status_received_ = false;

	output_ << (u8_t)E_CMD_STATUS;
	sync_write();
}

void client::send_load_program(const u8_t prog_id, const string& filename) {
	if ( !connected_ ) return;
	
	boost::mutex::scoped_lock lock(output_mutex_);

	output_ << (u8_t)E_CMD_LOAD;
	output_ << (u8_t)prog_id;
	output_ << (u8_t)filename.size();
	output_ << filename;
	sync_write();
}

void client::send_run_program(const u8_t spu_index, const u8_t prog_id) {
	if ( !connected_ ) return;
	
	boost::mutex::scoped_lock lock(output_mutex_);

	output_ << (u8_t)E_CMD_RUN;
	output_ << (u8_t)spu_index;
	output_ << (u8_t)prog_id;
	sync_write();
}

void client::send_parameter(const u8_t id, const parameter& param) {
	if ( !connected_ ) return;

	boost::mutex::scoped_lock lock(output_mutex_);

	const size_t value16 = htonl(param.parameter_size());
	
	output_ << (u8_t)E_CMD_PARAMETER;
	output_ << (u8_t)id;
	output_.write( (const char*)&value16, sizeof(size_t) );
	output_.write( (const char*)param.parameter_data(), param.parameter_size() );
	sync_write();
		
	cout << "client: parameter send length = " << param.parameter_size() << endl;
}

void client::send_sequence(const u8_t id, const sequence_t& seq) {
	if ( !connected_ ) return;

	boost::mutex::scoped_lock lock(output_mutex_);

	cout << "client: send sequence: length = " << (int)seq.size() << " total size = " << (seq.size() * 2 * sizeof(u8_t)) << endl;
	
	output_ << (u8_t)E_CMD_SEQUENCE;
	output_ << (u8_t)id;
	output_ << (u8_t)seq.size();
	for (u8_t i = 0; i < seq.size(); i++) {
		output_ << (u8_t)seq[i].first;
		output_ << (u8_t)seq[i].second;
	}
	sync_write();
}

void client::send_chunk(const chunk_t* chunk) {
	if ( !connected_ ) return;

	boost::mutex::scoped_lock lock(output_mutex_);
		
	output_ << (u8_t)E_CMD_CHUNK;
	output_.write( (const char*)chunk, sizeof(chunk_t) );
	sync_write();

	send_stat_.now();
}

void client::send_shutdown(void) {
	if ( !connected_ ) return;

	boost::mutex::scoped_lock lock(output_mutex_);

	output_ << (u8_t)E_CMD_SHUTDOWN;
	sync_write();

	disconnect();
}

void client::handle_read(const boost::system::error_code& error) {
	if ( !connected_ ) return;

	if (error) {
		if ( error == boost::asio::error::eof ) {
			cerr << "client: socket closed ";
		} else {
			cerr << "client: read error: " << error << endl;
		}
		return;
	}

	u8_t		cmd;
	chunk_t*	chunk;
	chunk_t		dummy;
	
	sync_read( sizeof(u8_t) );
	input_ >> cmd;
	switch (cmd) {
	case E_CMD_STATUS:
		cout << "client: status received" << endl;

		sync_read( sizeof(status_t) );
		input_.read( (char*)&server_status_, sizeof(status_t) );
		server_status_.buffer_free = ntohl(server_status_.buffer_free);
		server_status_.buffer_wait = ntohl(server_status_.buffer_wait);
		server_status_.buffer_busy = ntohl(server_status_.buffer_busy);
		server_status_.buffer_ready = ntohl(server_status_.buffer_ready);

		server_status_received_ = true;

		break;
	case E_CMD_CHUNK:
		sync_read( sizeof(chunk_t) );

		chunk = new chunk_t();
		if ( chunk != NULL ) {
			recv_stat_.now();

			input_.read( (char*)chunk, sizeof(chunk_t) );

//			cout << "client: chunk received (id " << chunk->id << ")" << endl;

			on_chunk_received_(chunk);

			delete chunk;
		} else {
			input_.read( (char*)&dummy, sizeof(chunk_t) );
			cerr << "Cannot allocate memory (chunk " << dummy.id << " lost)" << endl;
		}
				
		break;
	default:
		cerr << "client: unknown command received" << endl;
		break;	
	}
	
	async_read( sizeof(u8_t) );
}

void client::sync_write(void) {
	output_.flush();
	boost::asio::write( socket_, output_buf_ );
}

void client::async_write(void) {

}

void client::sync_read(const size_t size) {
	boost::system::error_code	error;
	
	if ( input_buf_.size() < size ) {
		boost::asio::read( socket_, input_buf_, boost::asio::transfer_at_least(size - input_buf_.size()), error );
		if ( error ) throw boost::system::system_error(error);
	}
}

void client::async_read(const size_t size) {
	if ( input_buf_.size() < size ) {
		boost::asio::async_read(
			socket_,
			input_buf_,
			boost::asio::transfer_at_least( size - input_buf_.size() ),
	        boost::bind(
	        	&client::handle_read,
	        	this,
				boost::asio::placeholders::error
			)
		);
	} else {
		handle_read(boost::system::error_code());
	}
}

}
