
#include <iostream>
#include <span>
#include <thread>
#include <cstddef>
#include <cstdlib>
#include "BidirectionalMultimessageSimulation.hpp"


namespace sns = simple_network_simulation;

namespace simple_network_simulation
{

bool initialize_program( const std::span<const char* const> command_line_arguments );

}


void inline static launch( int argc, char* argv[] )
{
	using std::size_t;
	using std::uint32_t;

	const std::span<const char* const> command_line_arguments { argv, static_cast<size_t>( argc ) };

	const bool hasError { sns::initialize_program( command_line_arguments ) };
	if ( hasError )
	{
		std::cerr << "exiting program...\n\n";
		return;
	}

	std::cout << "\n\nConnection simulation started...\n\n\n";

	const uint32_t node1_process1_num { 5001 };
	const uint32_t node1_process2_num { 5002 };
	const uint32_t node2_process1_num { 7001 };
	const uint32_t node2_process2_num { 7002 };

	std::jthread connection1_thread { sns::execute_connection1, node1_process1_num, node2_process2_num };
	std::jthread connection2_thread { sns::execute_connection2, node1_process2_num, node2_process1_num };

	connection1_thread.join( );
	connection2_thread.join( );

	std::cout << "\nConnection simulation finished...\n\n\n" << std::flush;
}

int main( int argc, char* argv[] )
{
	const auto registration_result { std::atexit( [ ]( ){ std::clog << "Program execution ended.\n\n"; } ) };
	if ( registration_result != 0 )
	{
		std::cerr << "\nRegistration failed.\n\n";
		return EXIT_FAILURE;
	}

	std::jthread launch_thread { launch, argc, argv };

	return 0;
}
