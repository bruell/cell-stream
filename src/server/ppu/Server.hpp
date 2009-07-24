/**
 * Server functions
 */

#pragma once

#include "../css.h"
#include "../../protocol.h"
#include "buffer.hpp"
#include "spu_controller.hpp"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std;
using boost::asio::ip::tcp;

namespace cell_stream {

class server;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
	
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create(boost::asio::io_service& io_service, server& serv) {
		return pointer(new tcp_connection(io_service, serv));
	}

	tcp::socket& socket() { return socket_; }

	void start_handling(void);
	
private:
	tcp_connection(boost::asio::io_service& io_service, server& serv) :
		socket_(io_service),
		input_(&input_buf_), output_(&output_buf_),
		server_(serv) { }

	void	handle_read(const boost::system::error_code&);
	void	handle_write(void);
	
	void	sync_write(void);
	void	async_write(void);
	void	sync_read(const size_t);
	void	async_read(const size_t);
	
	bool	check_error(const boost::system::error_code&, const string&);
	
	tcp::socket				socket_;
	boost::asio::streambuf	input_buf_;
	istream					input_;
	boost::asio::streambuf	output_buf_;
	ostream					output_;
	boost::mutex			output_mutex_;
	
	server&					server_;
	boost::thread*			send_thread_;
};

class server {
	
public:
	server(
		cell_stream::spu_controller& spu_controller,
		const u16_t& port
	) :
		spu_controller_(spu_controller),
		acceptor_(io_service_, tcp::endpoint(tcp::v4(), port))
	{
		start_accept();
	}
	virtual ~server() { }
	
	cell_stream::buffer&			buffer(void)			{ return spu_controller_.buffer(); }
	cell_stream::spu_controller&	spu_controller(void)	{ return spu_controller_; }
	
	void	start(void);
	void	stop(void);
	void	join(void);

private:
	void start_accept(void);

	void handle_accept(tcp_connection::pointer, const boost::system::error_code&);

	cell_stream::spu_controller&	spu_controller_;

	boost::asio::io_service			io_service_;
	tcp::acceptor					acceptor_;
	
	boost::thread*					io_thread_;
		
};

}
