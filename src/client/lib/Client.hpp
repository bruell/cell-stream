/**
 * Cell stream client
 */

#pragma once

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "../../protocol.h"
#include "../../stat.hpp"
#include "chunk.hpp"
#include "program.hpp"

using namespace std;
using boost::asio::ip::tcp;

namespace cell_stream {

class client {

public:
    client() :
    	connected_(false),
    	resolver_(io_service_),
    	socket_(io_service_),
    	input_(&input_buf_), output_(&output_buf_)
    {
    	clear_statistics();
    }
	virtual ~client()	{ }
    
	void connect_chunk_source(chunk_source&);
	void connect_chunk_destination(chunk_destination&);
	
	void			clear_statistics(void);
	void			show_statistics(void);
	const status_t&	server_status(void);

    void connect(const string&, int);
	void disconnect(void);
	void send_status_request(void);
	void send_load_program(const u8_t, const string&);
	void send_run_program(const u8_t, const u8_t);
	void send_parameter(const u8_t, const parameter&);
	void send_sequence(const u8_t, const sequence_t&);
	void send_chunk(const chunk_t*);
	void send_shutdown(void);
	
private:
	void handle_read(const boost::system::error_code&);
	
	inline void	sync_write(void);
	inline void	async_write(void);
	inline void	sync_read(const size_t);
	inline void	async_read(const size_t);
	
	stat				recv_stat_;
	stat				send_stat_;
	
	bool				connected_;
	
	chunk_signal_t		on_chunk_received_;
	
	status_t			server_status_;
	bool				server_status_received_;
	
	boost::asio::io_service	io_service_;
	tcp::resolver			resolver_;
	tcp::socket				socket_;
	boost::mutex			output_mutex_;
	
	boost::asio::streambuf	input_buf_;
	istream					input_;
	boost::asio::streambuf	output_buf_;
	ostream					output_;
	
	boost::thread*			io_thread_;

};

}
