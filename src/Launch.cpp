
#include <span>
#include <system_error>
#include <string_view>
#include <thread>
#include <functional>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <fmt/core.h>
#include "Application.hpp"
#include "BidirectionalMultimessageSimulation.hpp"
#include "Util.hpp"


void inline static
launch( const std::span<const char* const> command_line_arguments, int& exit_code_OUT ) noexcept
{
	[[ nodiscard ]] std::error_condition
	initialize_program( const std::span<const char* const> command_line_arguments ) noexcept;

	namespace sns = simple_network_simulation;

	const std::error_condition err_cond { initialize_program( command_line_arguments ) };
	if ( err_cond )
	{
		if ( err_cond.value( ) == static_cast<int>( std::errc::operation_canceled ) ) [[ likely ]]
		{
			exit_code_OUT = EXIT_SUCCESS;
		}
		else if ( err_cond.value( ) == static_cast<int>( std::errc::io_error ) ) [[ unlikely ]]
		{
			exit_code_OUT = EXIT_FAILURE;
		}
		else if ( err_cond.value( ) == static_cast<int>( std::errc::invalid_argument ) ) [[ likely ]]
		{
			exit_code_OUT = EXIT_FAILURE;
			constexpr std::string_view custom_error_message { "Invalid command-line argument" };
			try
			{
				fmt::print( stderr, "\n{0}: error: {1}: {2}\n\nexiting program...\n\n",
							sns::application_name, err_cond.value( ), custom_error_message );
			}
			catch ( ... ) { }
		}
		else [[ unlikely ]]
		{
			exit_code_OUT = EXIT_FAILURE;
			try
			{
				fmt::print( stderr, "\n{0}: error: {1}: {2}\n\nexiting program...\n\n",
							sns::application_name, err_cond.value( ), err_cond.message( ) );
			}
			catch ( ... ) { }
		}

		return;
	}

	try
	{
		fmt::print( "\n\nConnection simulation started...\n\n\n" );
		std::fflush( stdout );
		if ( std::ferror( stdout ) ) [[ unlikely ]]
			throw std::system_error { std::make_error_code( std::errc::io_error ) };

		{
			const std::uint32_t node1_process1_num { 5001 };
			const std::uint32_t node1_process2_num { 5002 };
			const std::uint32_t node2_process1_num { 7001 };
			const std::uint32_t node2_process2_num { 7002 };

			std::jthread connection1_thread { sns::execute_connection1, node1_process1_num, node2_process2_num };

			std::jthread connection2_thread { sns::execute_connection2, node1_process2_num, node2_process1_num };
		}

		fmt::print( "\nConnection simulation finished...\n\n\n" );
		std::fflush( stdout );
		if ( std::ferror( stdout ) ) [[ unlikely ]]
			throw std::system_error { std::make_error_code( std::errc::io_error ) };

		exit_code_OUT = EXIT_SUCCESS;
	}
	catch ( ... )
	{
		exit_code_OUT = EXIT_FAILURE;
	}

	return;
}

[[ nodiscard ]] int inline static
initiate( const int argc, const char* const* const argv ) noexcept
{
	int exit_code { };

	const auto registration_result { std::atexit( [ ]( )
												  {
													fmt::print( stderr, "Program execution ended.\n\n" );
												  } ) };
	if ( registration_result != 0 ) [[ unlikely ]]
	{
		exit_code = EXIT_FAILURE;

		try
		{
			fmt::print( stderr, "\nRegistration failed.\n\n" );
		}
		catch ( ... ) { }

		return exit_code;
	}

	const std::span<const char* const> command_line_arguments { argv, static_cast<std::size_t>( argc ) };

	try
	{
		std::jthread launch_thread { launch, command_line_arguments, std::ref( exit_code ) };
		launch_thread.join( );
	}
	catch ( ... )
	{
		exit_code = EXIT_FAILURE;
	}

	return exit_code;
}

int main( int argc, char* argv[] )
{
	return initiate( argc, argv );
}
