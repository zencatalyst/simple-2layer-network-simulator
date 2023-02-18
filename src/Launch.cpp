
#include <span>
#include <system_error>
#include <exception>
#include <string_view>
#include <thread>
#include <functional>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fmt/core.h>
#include <fmt/chrono.h>
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
			spdlog::get( "basic_logger" )->critical( "{}", err_cond.message( ) );
			try
			{
				fmt::print( stderr, "Something went wrong!\n\n" );
			}
			catch ( const std::exception& ex )
			{
				spdlog::get( "basic_logger" )->critical( "{}", ex.what( ) );
			}
		}
		else if ( err_cond.value( ) == static_cast<int>( std::errc::invalid_argument ) ) [[ likely ]]
		{
			exit_code_OUT = EXIT_FAILURE;
			constexpr std::string_view custom_error_message { "Invalid command-line argument" };
			spdlog::get( "basic_logger" )->critical( "{}", custom_error_message );
			try
			{
				fmt::print( stderr, "\n{0}: error: {1}: {2}\n\nexiting program...\n\n",
							sns::application_name, err_cond.value( ), custom_error_message );
			}
			catch ( const std::exception& ex )
			{
				spdlog::get( "basic_logger" )->critical( "{}", ex.what( ) );
			}
		}
		else [[ unlikely ]]
		{
			exit_code_OUT = EXIT_FAILURE;
			spdlog::get( "basic_logger" )->critical( "{}", err_cond.message( ) );
			try
			{
				fmt::print( stderr, "Something went wrong!\n\n" );
			}
			catch ( const std::exception& ex )
			{
				spdlog::get( "basic_logger" )->critical( "{}", ex.what( ) );
			}
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
	catch ( const std::exception& ex )
	{
		exit_code_OUT = EXIT_FAILURE;
		spdlog::get( "basic_logger" )->critical( "{}", ex.what( ) );
		try
		{
			fmt::print( stderr, "Something went wrong!\n\n" );
		}
		catch ( const std::exception& exc )
		{
			spdlog::get( "basic_logger" )->critical( "{}", exc.what( ) );
		}
	}

	return;
}

[[ nodiscard ]] bool inline static
register_exit_handlers( ) noexcept
{
	constexpr auto quick_exit_handler { [ ]( )
										{
											fmt::print( "Program execution ended.\n\n" );
										} };

	constexpr auto exit_handler { [ ]( )
								  {
									spdlog::get( "basic_logger" )->info( "Program execution ended." );
									fmt::print( "Program execution ended.\n\n" );
								  } };

	bool registration_failed;

	try
	{
		const auto first_registration_result { std::at_quick_exit( quick_exit_handler ) };
		if ( first_registration_result != 0 ) [[ unlikely ]]
			throw std::system_error { std::make_error_code( std::errc::operation_canceled ),
									  "Quick exit handler registration failed" };

		const auto second_registration_result { std::atexit( exit_handler ) };
		if ( second_registration_result != 0 ) [[ unlikely ]]
			throw std::system_error { std::make_error_code( std::errc::operation_canceled ),
									  "Exit handler registration failed" };

		registration_failed = false;
	}
	catch ( const std::system_error& se )
	{
		fmt::print( stderr, "{}\n\n", se.what( ) );
		registration_failed = true;
	}

	return registration_failed;
}

[[ nodiscard ]] bool inline static
register_loggers( ) noexcept
{
	spdlog::file_event_handlers handlers;

	handlers.after_open   = []( const spdlog::filename_t filename, std::FILE* const stream )
							{ fmt::print( stream, "\nFile '{}': Logging started...\n\n", filename ); };

	handlers.before_close = []( const spdlog::filename_t filename, std::FILE* const stream )
							{ fmt::print( stream, "\nFile '{}': Logging finished.\n", filename ); };

	bool registration_failed;

	try
	{
		auto logger { spdlog::basic_logger_st( "basic_logger", "logs/basic-log.txt", true, handlers ) };
		registration_failed = false;
	}
	catch ( const spdlog::spdlog_ex& sx )
	{
		fmt::print( stderr, "Log file init failed: {}\n\n", sx.what( ) );
		registration_failed = true;
	}

	return registration_failed;
}

[[ nodiscard ]] int inline static
initiate( const int argc, const char* const* const argv ) noexcept
{
	int exit_code { };

	if ( const bool loggers_registration_result { register_loggers( ) };
		 loggers_registration_result ) [[ unlikely ]]
	{
		exit_code = EXIT_FAILURE;
		std::quick_exit( exit_code );
	}

	if ( const bool exit_handlers_registration_result { register_exit_handlers( ) };
		 exit_handlers_registration_result ) [[ unlikely ]]
	{
		exit_code = EXIT_FAILURE;
		std::exit( exit_code );
	}

	const std::span<const char* const> command_line_arguments { argv, static_cast<std::size_t>( argc ) };

	try
	{
		std::jthread launch_thread { launch, command_line_arguments, std::ref( exit_code ) };
		launch_thread.join( );
	}
	catch ( const std::exception& ex )
	{
		exit_code = EXIT_FAILURE;
		spdlog::get( "basic_logger" )->critical( "{}", ex.what( ) );
		try
		{
			fmt::print( stderr, "Something went wrong!\n\n" );
		}
		catch ( const std::exception& exc )
		{
			spdlog::get( "basic_logger" )->critical( "{}", exc.what( ) );
		}
	}

	return exit_code;
}

int main( int argc, char* argv[] )
{
	return initiate( argc, argv );
}
