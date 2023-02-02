
#include "Application.hpp"
#include <system_error>
#include <span>
#include <string_view>
#include <cstdio>
#include <fmt/core.h>


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
}


void
set_layers_delays( const bool layers_delays_status ) noexcept;

void
set_channel_faults( const bool channel_faults_status ) noexcept;

}

[[ nodiscard ]] std::error_condition
initialize_program( const std::span<const char* const> command_line_arguments ) noexcept
{
	using std::string_view_literals::operator""sv;

	constexpr auto layers_delays_on_short_arg  { "-d"sv };
	constexpr auto layers_delays_on_long_arg   { "--layers-delays=on"sv };
	constexpr auto layers_delays_off_long_arg  { "--layers-delays=off"sv };
	constexpr auto channel_faults_on_short_arg { "-f"sv };
	constexpr auto channel_faults_on_long_arg  { "--channel-faults=on"sv };
	constexpr auto channel_faults_off_long_arg { "--channel-faults=off"sv };
	constexpr auto display_version_arg         { "--version"sv };
	constexpr auto display_help_arg            { "--help"sv };

	const auto command_line_options { command_line_arguments.subspan( 1 ) };

	for ( const std::string_view option : command_line_options )
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
			}
			catch ( ... )
			{
				return std::error_condition { std::errc::io_error };
			}

			return std::error_condition { std::errc::operation_canceled };
		}
		else if ( option == display_help_arg )
		{
			try
			{
				sns::display_help( );
			}
			catch ( ... )
			{
				return std::error_condition { std::errc::io_error };
			}

			return std::error_condition { std::errc::operation_canceled };
		}
		else
		{
			return std::error_condition { std::errc::invalid_argument };
		}
	}

	return std::error_condition { };
}
