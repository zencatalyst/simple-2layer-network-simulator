
#include <system_error>
#include <span>
#include <thread>
#include <exception>
#include <functional>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include "BidirectionalMultimessageSimulation.hpp"
#include "Util.hpp"


extern constinit int exit_code { };


[[ nodiscard ]] std::error_condition
initialize_program( const std::span<const char* const> command_line_arguments ) noexcept;

[[ nodiscard ]] bool
register_loggers( ) noexcept;

[[ nodiscard ]] bool
register_exit_handlers( ) noexcept;


void inline static
launch( const std::span<const char* const> command_line_arguments, int& exit_code_OUT ) noexcept
{
    const std::error_condition init_result_code { initialize_program( command_line_arguments ) };
    if ( init_result_code )
    {
        if ( init_result_code.value( ) == static_cast<int>( std::errc::operation_canceled ) ) [[ likely ]]
        {
            exit_code_OUT = EXIT_SUCCESS;
            spdlog::get( "basic_logger" )->info( "{}", init_result_code.message( ) );
        }
        else [[ unlikely ]]
        {
            exit_code_OUT = EXIT_FAILURE;
            spdlog::get( "basic_logger" )->error( "{}", init_result_code.message( ) );
        }

        return;
    }

    try
    {
        namespace sns = simple_network_simulation;

        fmt::print( "\n\nConnection simulation started...\n\n\n" );
        sns::util::flush_stdout( );

        {
            const std::uint32_t node1_process1_num { 5001 };
            const std::uint32_t node1_process2_num { 5002 };
            const std::uint32_t node2_process1_num { 7001 };
            const std::uint32_t node2_process2_num { 7002 };

            std::jthread connection1_thread { sns::execute_connection1, node1_process1_num, node2_process2_num };

            std::jthread connection2_thread { sns::execute_connection2, node1_process2_num, node2_process1_num };
        }

        fmt::print( "\nConnection simulation finished...\n\n\n" );
        sns::util::flush_stdout( );

        exit_code_OUT = EXIT_SUCCESS;
    }
    catch ( const std::exception& ex )
    {
        exit_code_OUT = EXIT_FAILURE;
        spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
        try
        {
            fmt::print( stderr, "\nSomething went wrong!\n\n" );
        }
        catch ( const std::exception& exc )
        {
            spdlog::get( "basic_logger" )->error( "{}", exc.what( ) );
        }
    }

    return;
}

[[ nodiscard ]] int inline static
initiate( const int argc, const char* const* const argv ) noexcept
{
#if SNS_DEBUG == 1
    {
    using simple_network_simulation::util::ScopedTimer;
    const ScopedTimer timer { [ ]( const auto duration ) noexcept
                              {
                                  spdlog::get( "basic_logger" )->debug( "Timer took {}", duration );
                                  try
                                  {
                                      fmt::print( stderr, "\nTimer took {}\n", duration );
                                  }
                                  catch ( const std::exception& ex )
                                  {
                                      exit_code = EXIT_FAILURE;
                                      spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
                                  }
                              } };
#endif

    if ( const bool is_loggers_registration_successful { register_loggers( ) };
         is_loggers_registration_successful == false ) [[ unlikely ]]
    {
        exit_code = EXIT_FAILURE;
        std::quick_exit( exit_code );
    }

    if ( const bool is_exit_handlers_registration_successful { register_exit_handlers( ) };
         is_exit_handlers_registration_successful == false ) [[ unlikely ]]
    {
        exit_code = EXIT_FAILURE;
        std::quick_exit( exit_code );
    }

    const std::span<const char* const> command_line_arguments { argv, static_cast<std::size_t>( argc ) };

    try
    {
        std::jthread launch_thread { launch, command_line_arguments, std::ref( exit_code ) };
    }
    catch ( const std::exception& ex )
    {
        exit_code = EXIT_FAILURE;
        spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
        try
        {
            fmt::print( stderr, "\nSomething went wrong!\n\n" );
        }
        catch ( const std::exception& exc )
        {
            spdlog::get( "basic_logger" )->error( "{}", exc.what( ) );
        }
    }

#if SNS_DEBUG == 1
    }
#endif

    return exit_code;
}

int main( int argc, char* argv[] )
{
    return initiate( argc, argv );
}
