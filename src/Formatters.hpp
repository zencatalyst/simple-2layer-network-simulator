
#pragma once

#include <bitset>
#include <cstddef>
#include <fmt/core.h>


template <std::size_t N>
struct fmt::formatter< std::bitset<N> >
{
	constexpr auto parse( const fmt::format_parse_context& ctx )
	{
		return ctx.begin( );
	}

	auto format( const std::bitset<N>& value, fmt::format_context& ctx ) const
	{
		return fmt::format_to( ctx.out( ), "{}", value.to_string( ) );
	}
};
