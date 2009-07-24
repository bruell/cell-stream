
#include "CSApp.hpp"

vector<string> tokenize(const string& str, const string& delimiters) {
	vector<string> tokens;
    	
	// skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    	
	// find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while ( string::npos != pos || string::npos != lastPos ) {
		// found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		
		// skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		
		// find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

int main(int argc, char* argv[]) {

	cout << "### Cell Stream Server Control ###" << endl;

	cell_stream::client				client;
	cell_stream::image				image;
	cell_stream::image_directory	image_dir;
	cell_stream::mask				mask;
	cell_stream::sequence_t			seq;

	char				c;
	string				cmdline, cmd;
	vector< string >	param;
	
	try {
	
	bool exit = false;
	while ( !exit ) {
		cout << " > ";
		cmdline = "";
		do {
			cin.get(c);
			if ( isprint(c) )
				cmdline += c;
		} while ( c != '\n' );
		
		param = tokenize(cmdline, " ");
		if ( param.size() < 1 ) continue;
		cmd = param[0];
		 
		if ( cmd == "help" ) {
			cout << "--- HELP ---" << endl;
			cout << "open [host] [port]          - open the connection to the cell stream server" << endl;
			cout << "close                       - close the connection" << endl;
			cout << "status                      - queries the status from the cell stream server" << endl;
			cout << "mask <file>                 - set the mask for the 9x9 convolution" << endl;
			cout << "image <in-file> <out-file>  - processes an image" << endl;
			cout << "imagedir <in-dir> <out-dir> - sends a whole directory of images" << endl;
			cout << "shutdown                    - shutdown of the cell stream server" << endl;
			cout << "quit                        - close any open connection and quit the client program" << endl;
		}
		
		if ( cmd == "prog1" ) {
			cout << "Open connection to host allegro port 6120" << endl;
			client.connect("allegro", 6120);
	
			client.send_status_request();
			cout << "Server status:" << endl;
			cout << "\tBuffer free chunks:  " << client.server_status().buffer_free << endl;
			cout << "\tBuffer wait chunks:  " << client.server_status().buffer_wait << endl;
			cout << "\tBuffer busy chunks:  " << client.server_status().buffer_busy << endl;
			cout << "\tBuffer ready chunks: " << client.server_status().buffer_ready << endl;		

			client.send_load_program(0, "spu/conv9");

			client.send_run_program(0, 0);
			client.send_run_program(1, 0);
			client.send_run_program(2, 0);
			client.send_run_program(3, 0);
			client.send_run_program(4, 0);
			client.send_run_program(5, 0);

			mask.load_mask("test.mask");
			cout << mask;
			client.send_parameter(0, mask);

			seq.push_back(cell_stream::sequence_entry_t(0, 0));
			client.send_sequence(0, seq);

			client.connect_chunk_source(image);
			client.connect_chunk_destination(image);
			image.in_file("in.jpg");
			image.out_file("out.jpg");
			image.sequence_id(0);
			image.process_image();
		}
		
		if ( cmd == "prog2" ) {
			cout << "Open connection to host allegro port 6120" << endl;
			client.connect("allegro", 6120);
	
			client.send_status_request();
			cout << "Server status:" << endl;
			cout << "\tBuffer free chunks:  " << client.server_status().buffer_free << endl;
			cout << "\tBuffer wait chunks:  " << client.server_status().buffer_wait << endl;
			cout << "\tBuffer busy chunks:  " << client.server_status().buffer_busy << endl;
			cout << "\tBuffer ready chunks: " << client.server_status().buffer_ready << endl;		

			client.send_load_program(0, "spu/conv9");

			client.send_run_program(0, 0);
			client.send_run_program(1, 0);
			client.send_run_program(2, 0);
			client.send_run_program(3, 0);
			client.send_run_program(4, 0);
			client.send_run_program(5, 0);

			mask.load_mask("test.mask");
			cout << mask;
			client.send_parameter(0, mask);

			seq.push_back(cell_stream::sequence_entry_t(0, 0));
			client.send_sequence(0, seq);

			client.connect_chunk_source(image_dir);
			client.connect_chunk_destination(image_dir);	
			image_dir.in_dir("in");
			image_dir.out_dir("out");
			image_dir.sequence_id(0);
			image_dir.process_images();

		}

		if ( cmd == "open" ) {
			u16_t port	= 6120;
			string host	= "allegro";
			
			if ( param.size() > 2 ) {
				port = atoi(param[2].c_str());
			}
			if ( param.size() > 1 ) {
				host = param[1];
			}
			
			cout << "Open connection to host " << host << " port " << port << endl;
			client.connect(host, port);
		}

		if ( cmd == "close" ) {
			cout << "Close connection to server" << endl;
			client.disconnect();
		}

		if ( cmd == "status" ) {
			client.send_status_request();
			cout << "Server status:" << endl;
			cout << "\tBuffer free chunks:  " << client.server_status().buffer_free << endl;
			cout << "\tBuffer wait chunks:  " << client.server_status().buffer_wait << endl;
			cout << "\tBuffer busy chunks:  " << client.server_status().buffer_busy << endl;
			cout << "\tBuffer ready chunks: " << client.server_status().buffer_ready << endl;
		}
		
		if ( cmd == "load" ) {
			if ( param.size() < 3 ) {
				cerr << "Error: Not enought parameter" << endl;
				continue; 				
			}
			client.send_load_program(atoi(param[1].c_str()), param[2].c_str());			
		}

		if ( cmd == "run" ) {
			if ( param.size() < 3 ) {
				cerr << "Error: Not enought parameter" << endl;
				continue; 				
			}
			client.send_run_program(atoi(param[1].c_str()), atoi(param[2].c_str()));			
		}

		if ( cmd == "mask" ) {
			if ( param.size() < 2 ) {
				cerr << "Error: Not enought parameter" << endl;
				continue; 				
			}
			mask.load_mask(param[1]);
			cout << mask;
			client.send_parameter(5, mask);
		}

		if ( cmd == "image" ) {
			if ( param.size() < 3 ) {
				cerr << "Error: Not enought parameter" << endl;
				continue; 				
			}
			client.connect_chunk_source(image);
			client.connect_chunk_destination(image);
			image.in_file(param[1]);
			image.out_file(param[2]);
			image.process_image();
		}

		if ( cmd == "imagedir" ) {
			if ( param.size() < 3 ) {
				cerr << "Error: Not enought parameter" << endl;
				continue; 				
			}
			client.connect_chunk_source(image_dir);
			client.connect_chunk_destination(image_dir);
			image_dir.in_dir(param[1]);
			image_dir.out_dir(param[2]);
			image_dir.process_images();
		}

		if ( cmd == "clearstat" ) {
			client.clear_statistics();
		}

		if ( cmd == "stat" ) {
			client.show_statistics();

			cout << endl;
			cout << "Image statistics" << endl;
			cout << "\tAvg process duration: " << image.statistics().avg() << " us" << endl;			

			cout << endl;
			cout << "ImageDir statistics" << endl;
			cout << "\tAvg process duration: " << image_dir.avg_process_time() << " us" << endl;
		}
		
		if ( cmd == "shutdown" ) {
			client.send_shutdown();
			cout << "Shutdown server" << endl;
		}

		if ( cmd == "quit" ) {
			client.disconnect();
			cout << "Quit programm" << endl;
			exit = true;
		}

		cout << endl;

	}
	
	} catch(exception& e) {
		cerr << "Error: " << e.what() << endl;		
	} catch(string& s) {
		cerr << "Error Str: " << s << endl;		
	}
	
	return 0;
}

