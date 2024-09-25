// https://godbolt.org/z/zGafK8fz5
#include "Application.hpp"
#include <string_view>
#include <array>
#include <expected>
#include <bitset>
#include <system_error>
#include <span>
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


using std::size_t;

namespace simple_network_simulation
{

namespace
{

using std::string_view_literals::operator""sv;

constexpr auto options_with_args_count { 5uz };

constexpr auto init_file_long_option { "--init-file="sv };
constexpr auto layers_delays_on_long_option { "--layers-delays=on"sv };
constexpr auto layers_delays_off_long_option { "--layers-delays=off"sv };
constexpr auto channel_faults_on_long_option { "--channel-faults=on"sv };
constexpr auto channel_faults_off_long_option { "--channel-faults=off"sv };

constexpr auto options_without_args_count { 4uz };

constexpr auto layers_delays_on_short_option { "-d"sv };
constexpr auto channel_faults_on_short_option { "-f"sv };
constexpr auto display_help_option { "--help"sv };
constexpr auto display_version_option { "--version"sv };

constexpr auto options_total_count { options_with_args_count + options_without_args_count };

constexpr std::array supported_cli_options { init_file_long_option,
                                             layers_delays_on_long_option, layers_delays_off_long_option,
                                             channel_faults_on_long_option, channel_faults_off_long_option,
                                             layers_delays_on_short_option, channel_faults_on_short_option,
                                             display_help_option, display_version_option };

static_assert( std::size( supported_cli_options ) == options_total_count );

constexpr auto options_with_args_begin { std::cbegin( supported_cli_options ) };
constexpr auto options_with_args_end { std::next( options_with_args_begin, options_with_args_count ) };
constexpr auto options_without_args_begin { options_with_args_end };
constexpr auto options_without_args_end { std::next( options_without_args_begin, options_without_args_count ) };

constexpr std::span supported_cli_options_with_args { options_with_args_begin, options_with_args_count };
constexpr std::span supported_cli_options_without_args { options_without_args_begin, options_without_args_count };

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
display_version( )
{
    fmt::print( stdout,
R"({0} {1}
Copyright (C) 2023 zencatalyst.
License MIT: MIT License <https://opensource.org/licenses/MIT>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Developed by Kasra Hashemi.

)", application_name, application_version );
    util::flush_stdout( );
}


void
set_layers_delays( const bool layers_delays_status ) noexcept;

void
set_channel_faults( const bool channel_faults_status ) noexcept;

}

[[ nodiscard ]] std::expected< decltype( supported_cli_options )::const_iterator,
                               std::pair<std::errc, decltype( supported_cli_options )::const_iterator> >
is_cli_option_with_arg_correctly_spelled( const std::string_view cli_option ) noexcept
{
    std::expected< decltype( supported_cli_options )::const_iterator,
                   std::pair<std::errc, decltype( supported_cli_options )::const_iterator> > result { };

    auto iter_to_first_occurring_option { std::cend( supported_cli_options ) };
    auto pos_of_first_occurring_option { std::string_view::npos };

    for ( auto it { options_with_args_begin }; it < options_with_args_end; ++it )
    {
        const auto& supported_cli_option { *it };
        const auto pos { cli_option.find( supported_cli_option ) };
        if ( pos != std::string_view::npos )
        {
            if ( pos < pos_of_first_occurring_option )
            {
                pos_of_first_occurring_option = pos;
                iter_to_first_occurring_option = it;
            }
            else if ( pos == pos_of_first_occurring_option &&
                      std::size( supported_cli_option ) > std::size( *iter_to_first_occurring_option ) )
            {
                iter_to_first_occurring_option = it;
            }
        }
    }

    if ( pos_of_first_occurring_option == 0 )
    {
        *result = iter_to_first_occurring_option;
    }
    else if ( pos_of_first_occurring_option == std::string_view::npos )
    {
        result = std::unexpected { std::pair { std::errc::invalid_argument, iter_to_first_occurring_option } };
    }
    else
    {
        result = std::unexpected { std::pair { std::errc::argument_out_of_domain, iter_to_first_occurring_option } };
    }

    return result;
}

[[ nodiscard ]] std::expected< decltype( supported_cli_options )::const_iterator,
                               std::pair<std::errc, decltype( supported_cli_options )::const_iterator> >
is_cli_option_without_arg_correctly_spelled( const std::string_view cli_option ) noexcept
{
    std::expected< decltype( supported_cli_options )::const_iterator,
                   std::pair<std::errc, decltype( supported_cli_options )::const_iterator> > result { };

    bool is_cli_option_recognized { };

    for ( auto it { options_without_args_begin }; it < options_without_args_end; ++it )
    {
        const auto& supported_cli_option { *it };

        if ( const auto pos { cli_option.rfind( supported_cli_option ) };
             pos != std::string_view::npos )
        {
            if ( pos != 0 || ( pos == 0 && std::size( cli_option ) > std::size( supported_cli_option ) ) )
            {
                result = std::unexpected { std::pair { std::errc::argument_out_of_domain, it } };
            }
            else
            {
                *result = it;
            }

            is_cli_option_recognized = true;
            break;
        }
    }

    if ( is_cli_option_recognized == false )
    {
        result = std::unexpected { std::pair { std::errc::invalid_argument, std::cend( supported_cli_options ) } };
    }

    return result;
}

[[ nodiscard ]] std::expected< std::bitset<std::size( supported_cli_options )>, std::pair<std::errc, size_t> >
recognize_command_line_options( const std::span<const char* const> command_line_options ) noexcept

[[ nodiscard ]] auto
recognize_command_line_options( const std::span<const char* const> command_line_options ) noexcept
-> std::expected< std::array<size_t, std::size( supported_cli_options )>,
                  std::tuple<std::errc,
                             decltype( command_line_options )::const_iterator,
                             decltype( supported_cli_options )::const_iterator> >
{
    std::expected< std::array<size_t, std::size( supported_cli_options )>,
                              std::tuple<std::errc,
                                         decltype( command_line_options )::const_iterator,
                                         decltype( supported_cli_options )::const_iterator> > result { };

    auto& recognized_cli_options_occurrence_counts { *result };

    for ( const std::string_view cli_option : command_line_options )
    {
        bool is_cli_option_recognized { };
        bool is_cli_option_incorrectly_spelled { };

        if ( const auto result { is_cli_option_with_arg_correctly_spelled( cli_option ) };
             result.has_value( ) )
        {
            const auto iter_to_matching_supported_cli_option { *result };
            const auto idx { std::distance( std::cbegin( supported_cli_options ),
                                            iter_to_matching_supported_cli_option ) };
            ++recognized_cli_options_occurrence_counts[ idx ];
            is_cli_option_recognized = true;
        }
        else
        {
            if ( const auto [ err_code, iter_to_matching_supported_cli_option ] { result.error( ) };
                 err_code == std::errc::argument_out_of_domain )
            {
                is_cli_option_recognized = true;
                break;
            }
            else if ( err_code == std::errc::invalid_argument )
            {
            }
        }

        if ( is_cli_option_recognized == false )
        {
            if ( const auto result { is_cli_option_without_arg_correctly_spelled( cli_option ) };
                 result.has_value( ) )
            {
                const auto iter_to_matching_supported_cli_option { *result };
                const auto idx { std::distance( std::cbegin( supported_cli_options ),
                                                iter_to_matching_supported_cli_option ) };
                ++recognized_cli_options_occurrence_counts[ idx ];
                is_cli_option_recognized = true;
            }
            else
            {
                if ( const auto [ err_code, iter_to_matching_supported_cli_option ] { result.error( ) };
                     err_code == std::errc::argument_out_of_domain )
                {
                    is_cli_option_recognized = true;
                    break;
                }
                else if ( err_code == std::errc::invalid_argument )
                {
                }
            }
        }

        for ( auto idx { 0uz }; idx < std::size( supported_cli_options_without_args ); ++idx )
        {
            if ( const auto pos { cli_option.rfind( supported_cli_options_without_args[ idx ] ) };
                 pos != std::string_view::npos )
            {
                if ( pos != 0 || ( pos == 0 && std::size( cli_option ) >
                                               std::size( supported_cli_options_without_args[ idx ] ) ) )
                {
                    is_cli_option_incorrectly_spelled = true;
                }
                else
                {
                    is_cli_option_recognized = true;

                    if ( recognized_cli_options_flags[ idx ] == false )
                    {
                        recognized_cli_options_flags[ idx ] = true;
                    }
                    else
                    {
                        is_cli_option_duplicate = true;
                    }
                }

                break;
            }
        }

        for ( auto idx { 0uz }; idx < std::size( supported_cli_options_with_args ); ++idx )
        {
            if ( const auto pos { cli_option.rfind( supported_cli_options_with_args[ idx ] ) };
                 pos != std::string_view::npos )
            {
                if ( pos != 0 )
                {
                    is_cli_option_incorrectly_spelled = true;
                }
                else
                {
                    is_cli_option_recognized = true;

                    if ( recognized_cli_options_flags[ idx ] == false )
                    {
                        recognized_cli_options_flags[ idx ] = true;
                    }
                    else
                    {
                        is_cli_option_duplicate = true;
                    }
                }

                break;
            }
        }

        if ( is_cli_option_incorrectly_spelled == true )
        {
            result = std::unexpected { std::errc::argument_out_of_domain };
            break;
        }

        if ( is_cli_option_recognized == false )
        {
            result = std::unexpected { std::errc::invalid_argument };
            break;
        }

        if ( is_cli_option_duplicate == true )
        {
            result = std::unexpected { std::errc::operation_not_permitted };
            break;
        }
    }

    return result;
}

[[ nodiscard ]] std::error_condition
initialize_program( const std::span<const char* const> command_line_arguments ) noexcept
{
    std::error_condition initialization_result_code { };

    if ( std::empty( command_line_arguments ) ) [[ unlikely ]] return initialization_result_code;

    const auto command_line_options { command_line_arguments.subspan( 1 ) };
    const auto result { recognize_command_line_options( command_line_options ) };

    for ( const auto command_line_options { command_line_arguments.subspan( 1 ) };
          const std::string_view option : command_line_options )
    {
        namespace sns = simple_network_simulation;
        using std::string_view_literals::operator""sv;
        constexpr auto unrecognized_option_message { "unrecognized command-line option"sv };
        constexpr auto invalid_combination_message { "invalid combination of command-line options"sv };
        constexpr auto guiding_message { "See ‘--help’ for more info on how to use the program"sv };

        

        if ( const auto pos { option.rfind( init_file_long_arg ) }; pos != std::string_view::npos )
        {
            if ( pos != 0 )
            {
                spdlog::get( "basic_logger" )->error( "{}", unrecognized_option_message );
                initialization_result_code = std::errc::invalid_argument;
                try
                {
                    fmt::print( stderr, "\n{0}: error: {1}: {2} ‘{3}’ (did you mean: {4})\n{5}\n\n",
                                sns::application_name, initialization_result_code.value( ),
                                unrecognized_option_message, option, init_file_long_arg, guiding_message );
                }
                catch ( const std::exception& ex )
                {
                    spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
                }

                break;
            }

            if ( const auto filename { option.substr( std::size( init_file_long_arg ) ) };
                 std::empty( filename ) )
            {
                constexpr auto no_filename_specified_message { "no init-file name/path specified"sv };
                spdlog::get( "basic_logger" )->error( "{}", no_filename_specified_message );
                initialization_result_code = std::errc::invalid_argument;
                constexpr auto instructing_message { "please specify an initialization file"sv };
                try
                {
                    fmt::print( stderr, "\n{0}: error: {1}: {2} in ‘{3}’ ({4})\n{5}\n\n",
                                sns::application_name, initialization_result_code.value( ),
                                no_filename_specified_message, option, instructing_message, guiding_message );
                }
                catch ( const std::exception& ex )
                {
                    spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
                }

                break;
            }
            else
            {
                std::filesystem::path filepath { };
                try
                {
                    filepath = std::filesystem::path { filename };

                    if ( !std::filesystem::exists( filepath ) )
                    {
                    }
                    else if ( !std::filesystem::is_regular_file( filepath ) )
                    {
                    }
                    else if ( !filepath.has_filename( ) )
                    {
                    }
                    else if ( filepath.extension() != "" && filepath.extension() != ".txt" )
                    {
                    }
                    else
                    {
                        std::ifstream file_stream { filepath };
                        if ( !file_stream.is_open( ) )
                        {
                        }

                        
                    }
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
                }

                break;
            }
        }
        else if ( option == layers_delays_on_short_arg || option == layers_delays_on_long_arg )
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
            if ( std::size( command_line_options ) > 1 )
            {
                spdlog::get( "basic_logger" )->error( "{}", invalid_combination_message );
                initialization_result_code = std::errc::invalid_argument;
                try
                {
                    fmt::print( stderr, "\n{0}: error: {1}: {2} ‘{3}’ and ({4})\n{5}\n\n",
                                sns::application_name, initialization_result_code.value( ),
                                invalid_combination_message, option, init_file_long_arg, guiding_message );
                }
                catch ( const std::exception& ex )
                {
                    spdlog::get( "basic_logger" )->error( "{}", ex.what( ) );
                }

                break;
            }
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
            spdlog::get( "basic_logger" )->error( "{}", unrecognized_option_message );
            initialization_result_code = std::errc::invalid_argument;
            try
            {
                fmt::print( stderr, "\n{0}: error: {1}: {2} ‘{3}’\n{4}\n\n",
                            sns::application_name, initialization_result_code.value( ),
                            unrecognized_option_message, option, guiding_message );
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
