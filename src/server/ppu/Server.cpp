
#include "server.hpp"

namespace cell_stream {

void tcp_connection::start_handling(void) {
	cout << "server: client connected" << endl;

	send_thread_ = new boost::thread(
		boost::bind( &tcp_connection::handle_write, this )
	); 

	async_read(1);
}

void tcp_connection::handle_read(const boost::system::error_code& error) {
	if ( check_error(error, "server: socket read error: ") ) return;
	
	u8_t		i, command, prog_id, spu_id, param_id, seq_id, length;
	char*		str;
	chunk_t*	chunk;
	chunk_t		null_chunk;
	u8_t*		data;
	size_t		size;
	
	input_ >> command;
	switch ( command ) {
	case E_CMD_STATUS:
		cout << "server: status request received" << endl;
		
		status_t status;
		status.buffer_free	= 100;
		status.buffer_wait	= 200;
		status.buffer_busy	= 300;
		status.buffer_ready	= 400;

		{
			boost::mutex::scoped_lock lock(output_mutex_);
			output_ << (u8_t)E_CMD_STATUS;
			output_.write( (const char*)&status, sizeof(status_t) );
			sync_write();
		}
		
		break;

	case E_CMD_LOAD:
		sync_read( sizeof(u8_t) + sizeof(u8_t) );
		input_.read( (char*)&prog_id, sizeof(u8_t) );
		input_.read( (char*)&length, sizeof(u8_t) );

		str = new char[ length + 1 ];
		
		sync_read( length );
		input_.read( str, length );
		str[length] = '\0';

		server_.spu_controller().program_load( prog_id, string(str) );
	
		delete str;
	
		break;

	case E_CMD_RUN:
		sync_read( sizeof(u8_t) + sizeof(u8_t) );
		input_.read( (char*)&spu_id, sizeof(u8_t) );
		input_.read( (char*)&prog_id, sizeof(u8_t) );

		server_.spu_controller().program_run( spu_id, prog_id );
		
		break;
		
	case E_CMD_PARAMETER:
		sync_read( sizeof(u8_t) + sizeof(size_t) );
		input_.read( (char*)&param_id, sizeof(u8_t) );
		input_.read( (char*)&size, sizeof(size_t) );
		
		data = server_.spu_controller().parameter_alloc(param_id, size);
		
		sync_read( size );
		input_.read( (char*)data, size );
		
		cout << "server: parameter received (id " << (int)param_id << " size " << (int)size << ")" << endl;

		break;

	case E_CMD_SEQUENCE:
		sync_read( sizeof(u8_t) + sizeof(u8_t) );
		input_.read( (char*)&seq_id, sizeof(u8_t) );
		input_.read( (char*)&length, sizeof(u8_t) );
		
		for ( i = 0; i < length; i++ ) {
			input_.read( (char*)&prog_id, sizeof(u8_t) );
			input_.read( (char*)&param_id, sizeof(u8_t) );
			server_.spu_controller().sequence_add_entry( seq_id, prog_id, param_id );
		}

		cout << "server: sequence received (id " << (int)seq_id << " length " << (int)length << ")" << endl;

		break;

	case E_CMD_CHUNK:
		sync_read( sizeof(chunk_t) );

		chunk = server_.buffer().find_chunk( E_CHUNK_FREE, E_CHUNK_RECEIVING );
		if ( chunk ) {
			input_.read( (char*)chunk, sizeof(chunk_t) );
			server_.spu_controller().chunk_cache_values(chunk);
			server_.buffer().step_chunk( chunk, E_CHUNK_RECEIVING, E_CHUNK_WAIT );
//			cout << "server: chunk received" << endl;
		} else {
			input_.read( (char*)&null_chunk, sizeof(chunk_t) );
			cerr << "server: couldn't find a free buffer slot for chunk (id " << (int)null_chunk.id << ")" << endl;
		}

		break;

	case E_CMD_SHUTDOWN:
		cout << "server: shutdown" << endl;
		server_.stop();

		break;
		
	default:
//		cout << "server: received unknown command: 0x" << hex << (int)command << endl;
		break;
	}

	// Listen to the next command
	async_read(1);
}

void tcp_connection::handle_write(void) {
	chunk_t* chunk;
	while ( socket_.is_open() ) {
		
		chunk = server_.buffer().find_chunk( E_CHUNK_READY, E_CHUNK_SENDING );
		if ( chunk == NULL ) continue;
		
		{
			boost::mutex::scoped_lock lock(output_mutex_);
		
			output_ << (u8_t)E_CMD_CHUNK;
			output_.write( (const char*)chunk, sizeof(chunk_t) );
			sync_write();
		}
		
		server_.buffer().step_chunk( chunk, E_CHUNK_SENDING, E_CHUNK_FREE );
		
	} 
}

void tcp_connection::sync_write(void) {
	output_.flush();
	boost::asio::write(socket_, output_buf_);
}

void tcp_connection::async_write(void) {

}

void tcp_connection::sync_read(const size_t size) {
	boost::system::error_code error;
	
	if ( input_buf_.size() < size ) {
		boost::asio::read(
			socket_,
			input_buf_,
			boost::asio::transfer_at_least( size - input_buf_.size() ),
			error
		);
		check_error(error, "server: socket read error: ");
	}
}

void tcp_connection::async_read(const size_t size) {
	if ( input_buf_.size() < size ) {
		boost::asio::async_read(
			socket_,
			input_buf_,
			boost::asio::transfer_at_least( size - input_buf_.size() ),
	        boost::bind(
	        	&tcp_connection::handle_read,
	        	shared_from_this(),
				boost::asio::placeholders::error
			)
		);
	} else {
		handle_read(boost::system::error_code());
	}
}

bool tcp_connection::check_error(const boost::system::error_code& error, const string& prefix) {
	bool result = false;
	
	if ( error ) {
		if ( error == boost::asio::error::eof ) {
			cout << "server: socket closed" << endl;
		} else {
			cerr << prefix << error << endl;
		}
		result = true;
		socket_.close();
	}
	
	return result;
}

void server::start_accept(void) {
	tcp_connection::pointer connection = tcp_connection::create(io_service_, *this);

	acceptor_.async_accept(
		connection->socket(),
		boost::bind(
			&server::handle_accept,
			this,
			connection,
			boost::asio::placeholders::error
		)
	);
}

void server::handle_accept(tcp_connection::pointer connection, const boost::system::error_code& error) {
	if (error) {
		cerr << "server: error accepting connection: " << error << endl;
	} else {
		connection->start_handling();
		start_accept();
	}
}

void server::start(void) {
	cout << "server: accepting connections on port " << acceptor_.local_endpoint().port() << endl;

	io_thread_ = new boost::thread(
		boost::bind( &boost::asio::io_service::run, &io_service_ )
	);
}

void server::stop(void) {
	io_service_.stop();
}

void server::join(void) {
	io_thread_->join();
}

}
