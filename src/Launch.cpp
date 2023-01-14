
#include <iostream>
#include <system_error>
#include <span>
#include <syncstream>
#include <thread>
#include <functional>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <fmt/core.h>
#include "BidirectionalMultimessageSimulation.hpp"


namespace sns = simple_network_simulation;

namespace simple_network_simulation
{

[[ nodiscard ]] std::error_condition
initialize_program( const std::span<const char* const> command_line_arguments ) noexcept;

}


void inline static launch( const int argc, const char* const* const argv, int& exit_code_OUT )
{
	using std::size_t;
	using std::uint32_t;

	const std::span<const char* const> command_line_arguments { argv, static_cast<size_t>( argc ) };

	const std::error_condition err_cond { sns::initialize_program( command_line_arguments ) };
	if ( err_cond )
	{
		exit_code_OUT = EXIT_FAILURE;
		fmt::print( stderr, "\n{}\n\nexiting program...\n\n", err_cond.message( ) );
		return;
	}

	fmt::print( "\n\nConnection simulation started...\n\n\n" );
	std::fflush( stdout );

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

	std::flush( std::cout );

	fmt::print( "\nConnection simulation finished...\n\n\n" );
	std::fflush( stdout );

	exit_code_OUT = EXIT_SUCCESS;
	return;
}

int main( int argc, char* argv[] )
{
	int exit_code { };

	const auto registration_result { std::atexit( [ ]( ){ fmt::print( stderr, "Program execution ended.\n\n" ); } ) };
	if ( registration_result != 0 )
	{
		fmt::print( stderr, "\nRegistration failed.\n\n" );
		return exit_code = EXIT_FAILURE;
	}

	std::jthread launch_thread { launch, argc, argv, std::ref( exit_code ) };
	launch_thread.join( );

	return exit_code;
}
