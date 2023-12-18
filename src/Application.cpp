
#include "Application.hpp"
#include <system_error>
#include <span>
#include <string_view>
#include <exception>
#include <format>
#include <filesystem>
#include <cstdio>
#include <cstdlib>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <glib.h>
#include "Util.hpp"


namespace simple_network_simulation
{

void
display_version( )
{
    fmt::print( stdout,
R"({0} {1}
Copyright (C) 2023 zencatalyst.
License MIT: MIT License <https://opensource.org/licenses/MIT>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written by Kasra Hashemi.

)", application_name, application_version );
    util::flush_stdout( );
}

void
display_help( )
{
    fmt::print( stdout,
R"(Usage: {0} [OPTION]...
Perform a bidirectional multi-message network
simulation (between 4 processes across 2 nodes).
Example: ./{0} --channel-faults=on

Options:
  -d, --layers-delays=on      add delays to the execution of the layers by
                              putting them to sleep for short amounts of time
      --layers-delays=off     do not add delays to the execution of the layers
                              (enabled by default)

  -f, --channel-faults=on     set the channel to have faults occasionally
                              when transferring bits from one node to the
                              other causing the dependent ongoing connection
                              to be closed
      --channel-faults=off    set the channel to have no faults
                              (enabled by default)

      --help       display this help and exit
      --version    output version information and exit

Both --layers-delays and --channel-faults default to 'off' if not provided.

Exit status:
 0  if OK,
 1  if minor problems (e.g., invalid command-line argument),
 2  if serious trouble (e.g., cannot access command-line argument).

Web <https://github.com/zencatalyst/simple-2layer-network-simulator>
Documentation <https://github.com/zencatalyst/simple-2layer-network-simulator/blob/main/README.md>

)", application_name );
    util::flush_stdout( );
}


void
set_layers_delays( const bool layers_delays_status ) noexcept;

void
set_channel_faults( const bool channel_faults_status ) noexcept;

}

[[ nodiscard ]] std::error_condition
initialize_program( const std::span<const char* const> command_line_arguments ) noexcept
{
    std::error_condition initialization_result_code { };

    if ( std::empty( command_line_arguments ) ) [[ unlikely ]] return initialization_result_code;

    using std::string_view_literals::operator""sv;

    constexpr auto layers_delays_on_short_arg  { "-d"sv };
    constexpr auto layers_delays_on_long_arg   { "--layers-delays=on"sv };
    constexpr auto layers_delays_off_long_arg  { "--layers-delays=off"sv };
    constexpr auto channel_faults_on_short_arg { "-f"sv };
    constexpr auto channel_faults_on_long_arg  { "--channel-faults=on"sv };
    constexpr auto channel_faults_off_long_arg { "--channel-faults=off"sv };
    constexpr auto display_version_arg         { "--version"sv };
    constexpr auto display_help_arg            { "--help"sv };

    for ( const auto command_line_options { command_line_arguments.subspan( 1 ) };
          const std::string_view option : command_line_options )
    {
        namespace sns = simple_network_simulation;

        if ( option == layers_delays_on_short_arg || option == layers_delays_on_long_arg )
        {
            sns::set_layers_delays( true );
        }
        else if ( option == layers_delays_off_long_arg )
        {
            sns::set_layers_delays( false );
        }
        else if ( option == channel_faults_on_short_arg || option == channel_faults_on_long_arg )
        {
            sns::set_channel_faults( true );
        }
        else if ( option == channel_faults_off_long_arg )
        {
            sns::set_channel_faults( false );
        }
        else if ( option == display_version_arg )
        {
            try
            {
                sns::display_version( );
                initialization_result_code = std::errc::operation_canceled;
            }
            catch ( const std::exception& ex )
            {
                spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
                try
                {
                    fmt::print( stderr, "\nSomething went wrong!\n\n" );
                }
                catch ( const std::exception& exc )
                {
                    spdlog::get( "basic_logger" )->error( "{}", exc.what( ) );
                }

                initialization_result_code = std::errc::io_error;
            }

            break;
        }
        else if ( option == display_help_arg )
        {
            try
            {
                sns::display_help( );
                initialization_result_code = std::errc::operation_canceled;
            }
            catch ( const std::exception& ex )
            {
                spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
                try
                {
                    fmt::print( stderr, "\nSomething went wrong!\n\n" );
                }
                catch ( const std::exception& exc )
                {
                    spdlog::get( "basic_logger" )->error( "{}", exc.what( ) );
                }

                initialization_result_code = std::errc::io_error;
            }

            break;
        }
        else
        {
            constexpr auto custom_error_message { "unrecognized command-line option"sv };
            spdlog::get( "basic_logger" )->error( "{}", custom_error_message );
            initialization_result_code = std::errc::invalid_argument;
            try
            {
                fmt::print( stderr, "\n{0}: error: {1}: {2} ‘{3}’\n"
                                    "See ‘{4}’ for more info on how to use the program\n\n",
                            sns::application_name, initialization_result_code.value( ),
                            custom_error_message, option, display_help_arg );
            }
            catch ( const std::exception& ex )
            {
                spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
            }

            break;
        }
    }

    return initialization_result_code;
}

[[ nodiscard ]] bool
register_exit_handlers( ) noexcept
{
    extern constinit const int exit_code;

    constexpr auto quick_exit_handler { [ ]
                                        {
                                            spdlog::get( "basic_logger" )->critical(
                                              "Program terminated (exit code: {})", exit_code );
                                            try
                                            {
                                                fmt::print(
                                                  "Program terminated (exit code: {})\n\n", exit_code );
                                            }
                                            catch ( const std::exception& ex )
                                            {
                                                spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
                                            }

                                            try
                                            {
                                                simple_network_simulation::util::flush_all_streams( );
                                            }
                                            catch ( const std::system_error& se )
                                            {
                                                spdlog::get( "basic_logger" )->critical( "{}", se.what( ) );
                                                spdlog::get( "basic_logger" )->flush( );
                                                spdlog::shutdown( );
                                                throw;
                                            }

                                            spdlog::get( "basic_logger" )->flush( );
                                            spdlog::shutdown( );
                                        } };

    constexpr auto exit_handler   { [ ]
                                    {
                                        try
                                        {
                                            if ( exit_code == EXIT_SUCCESS )
                                            {
                                                spdlog::get( "basic_logger" )->info(
                                                  "Program execution ended (exit code: {})", exit_code );
                                                fmt::print(
                                                  "Program execution ended (exit code: {})\n\n", exit_code );
                                            }
                                            else
                                            {
                                                spdlog::get( "basic_logger" )->error(
                                                  "Program exited abnormally (exit code: {})", exit_code );
                                                fmt::print(
                                                  "Program exited abnormally (exit code: {})\n\n", exit_code );
                                            }
                                        }
                                        catch ( const std::exception& ex )
                                        {
                                            spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
                                        }

                                        try
                                        {
                                            simple_network_simulation::util::flush_all_streams( );
                                        }
                                        catch ( const std::system_error& se )
                                        {
                                            spdlog::get( "basic_logger" )->critical( "{}", se.what( ) );
                                            spdlog::get( "basic_logger" )->flush( );
                                            spdlog::shutdown( );
                                            throw;
                                        }

                                        spdlog::get( "basic_logger" )->flush( );
                                    } };

    bool is_registration_successful;

    try
    {
        const auto quick_exit_handler_registration_result { std::at_quick_exit( quick_exit_handler ) };
        if ( quick_exit_handler_registration_result != 0 ) [[ unlikely ]]
            throw std::system_error { std::make_error_code( std::errc::operation_canceled ),
                                      "Quick exit handler registration failed" };

        const auto exit_handler_registration_result { std::atexit( exit_handler ) };
        if ( exit_handler_registration_result != 0 ) [[ unlikely ]]
            throw std::system_error { std::make_error_code( std::errc::operation_canceled ),
                                      "Exit handler registration failed" };

        is_registration_successful = true;
    }
    catch ( const std::system_error& se )
    {
        spdlog::get( "basic_logger" )->error( "{}", se.what( ) );
        try
        {
            fmt::print( stderr, "\nSomething went wrong during program startup!\n\n" );
            simple_network_simulation::util::flush_stderr( );
        }
        catch ( const std::exception& ex )
        {
            spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
        }
        spdlog::get( "basic_logger" )->flush( );

        is_registration_successful = false;
    }

    return is_registration_successful;
}

[[ nodiscard ]] bool
register_loggers( ) noexcept( false )
{
    constexpr std::string_view dashes { "------------------------------------------------------------"
                                        "------------------------------------------------------------" };
    static_assert( std::size( dashes ) == 120 );

    constexpr std::string_view asterisks { "************************************************************"
                                           "************************************************************" };
    static_assert( std::size( asterisks ) == 120 );

    spdlog::file_event_handlers handlers;

    handlers.after_open   = [ dashes ]( const spdlog::filename_t filename, std::FILE* const stream )
                            {
                                using simple_network_simulation::util::retrieve_current_local_time;
                                fmt::print( stream, "\n[{0}] [{1}] Logging started...\n{2}\n",
                                            std::format( "{:%F (%a) %T %Ez %Z}", retrieve_current_local_time( ) ),
                                            filename,
                                            dashes );
                            };

    handlers.before_close = [ dashes, asterisks ]( const spdlog::filename_t filename, std::FILE* const stream )
                            {
                                using simple_network_simulation::util::retrieve_current_local_time;
                                fmt::print( stream, "{0}\n[{1}] [{2}] Logging finished.\n\n{3}\n",
                                            dashes,
                                            std::format( "{:%F (%a) %T %Ez %Z}", retrieve_current_local_time( ) ),
                                            filename,
                                            asterisks );
                            };

    const std::string_view user_state_dir { g_get_user_state_dir( ) };
    constexpr std::string_view app_logs_dir { "Simple-2Layer-Network-Simulator/logs" };
    constexpr std::string_view log_file_name { "basic-log.txt" };

    std::filesystem::path log_file_path;

    bool is_registration_successful;

    try
    {
        log_file_path /= user_state_dir;
        log_file_path /= app_logs_dir;
        log_file_path /= log_file_name;

        auto logger { spdlog::basic_logger_st( "basic_logger", log_file_path, false, handlers ) };

#if SNS_DEBUG == 0
        logger->set_level( spdlog::level::info );
        logger->set_pattern( "[%Y-%m-%d (%a) %T.%f %z] [%n] [thread %t] [%l] %v" );
#else
        logger->set_level( spdlog::level::debug );
        logger->set_pattern( "[%Y-%m-%d (%a) %T.%f %z] [%n] [thread %t] [%l] [%@] %v" );
#endif
        is_registration_successful = true;
    }
    catch ( const std::exception& ex )
    {
        fmt::print( stderr, "\nSomething went wrong during program startup: log file init failed: {}\n\n",
                    ex.what( ) );
        simple_network_simulation::util::flush_stderr( );
        is_registration_successful = false;
    }

    return is_registration_successful;
}
