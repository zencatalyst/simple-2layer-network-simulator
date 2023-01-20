
#pragma once

#include <bitset>
#include <cstddef>
#include <fmt/core.h>


template <std::size_t N>
struct fmt::formatter< std::bitset<N> >
{
	constexpr auto parse( format_parse_context& ctx )
	{
		return ctx.begin( );
	}

	auto format( const std::bitset<N>& value, format_context& ctx )
	{
		return fmt::format_to( ctx.out( ), "{}", value.to_string( ) );
	}
};
