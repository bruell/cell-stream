
#include "../css.h"
#include "buffer.hpp"
#include "server.hpp"
#include "spu_controller.hpp"

#include <ctype.h>
#include <getopt.h>
#include <iostream>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
	u16_t port = 6120;
	
	po::options_description desc("cell stream options");
	desc.add_options()
		("help", "print help message")
		("port", po::value< u16_t >(), "specifies the port the server is listen to")
		;
		
	po::variables_map values;
	po::store(po::parse_command_line(argc, argv, desc), values);
	po::notify(values);
	
	if ( values.count("help") ) {
		cout << desc << endl;
		return 1;
	}
	
	if ( values.count("port") ) {
		port = values["port"].as< u16_t >();		
	}
	
	cout << "chunk size: " << sizeof(chunk_t) << endl;

/*
	buffer_init();

	spu_parameter_init();

	spu_sequence_init();

	spu_controller_init();
*/
	cell_stream::buffer buffer;
	buffer.init(1024);
	
	cell_stream::spu_controller spu_controller(buffer);

	cell_stream::server server(spu_controller, port);
	server.start();
	server.join();

//	spu_controller_join();
	
	return 0;
}
