
#include <iostream>
#include <span>
#include <thread>
#include <functional>
#include <cstddef>
#include <cstdlib>
#include "BidirectionalMultimessageSimulation.hpp"


namespace sns = simple_network_simulation;

namespace simple_network_simulation
{

bool initialize_program( const std::span<const char* const> command_line_arguments, std::ostream& out_stream );

}


void inline static launch( const int argc, const char* const* const argv, int& exit_code_OUT )
{
	using std::size_t;
	using std::uint32_t;

	const std::span<const char* const> command_line_arguments { argv, static_cast<size_t>( argc ) };

	const bool hasError { sns::initialize_program( command_line_arguments, std::cerr ) };
	if ( hasError )
	{
		exit_code_OUT = EXIT_FAILURE;
		std::cerr << "exiting program...\n\n";
		return;
	}

	std::cout << "\n\nConnection simulation started...\n\n\n";

	{
		const uint32_t node1_process1_num { 5001 };
		const uint32_t node1_process2_num { 5002 };
		const uint32_t node2_process1_num { 7001 };
		const uint32_t node2_process2_num { 7002 };
		std::osyncstream connection1_thread_out_sync_stream { std::cout };
		std::osyncstream connection2_thread_out_sync_stream { std::cout };

		std::jthread connection1_thread { sns::execute_connection1, node1_process1_num, node2_process2_num,
										  std::ref( connection1_thread_out_sync_stream ) };

		std::jthread connection2_thread { sns::execute_connection2, node1_process2_num, node2_process1_num,
										  std::ref( connection2_thread_out_sync_stream ) };
	}

	std::cout << "\nConnection simulation finished...\n\n\n" << std::flush;

	exit_code_OUT = EXIT_SUCCESS;
	return;
}

int main( int argc, char* argv[] )
{
	int exit_code { };

	const auto registration_result { std::atexit( [ ]( ){ std::clog << "Program execution ended.\n\n"; } ) };
	if ( registration_result != 0 )
	{
		std::cerr << "\nRegistration failed.\n\n";
		return exit_code = EXIT_FAILURE;
	}

	std::jthread launch_thread { launch, argc, argv, std::ref( exit_code ) };
	launch_thread.join( );

	return exit_code;
}
