
#pragma once

#include <string_view>


namespace simple_network_simulation
{

inline constexpr std::string_view application_name { "Simple-2Layer-Network-Simulator" };
inline constexpr std::string_view application_version { "0.8.0" };


void
display_version( );

void
display_help( );

}
