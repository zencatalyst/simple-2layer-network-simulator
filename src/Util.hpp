
#pragma once

#include <chrono>
#include <cstdio>
#include <fmt/core.h>
#include <fmt/chrono.h>


namespace simple_network_simulation::util
{

template < class Time  = std::chrono::microseconds,
		   class Clock = std::chrono::steady_clock >
requires ( std::chrono::is_clock_v<Clock> )
struct ScopedTimer
{
	const std::chrono::time_point< Clock > m_start { Clock::now( ) };
		  std::chrono::time_point< Clock > m_end;
	std::FILE* m_stream;

	ScopedTimer( std::FILE* const stream = stderr )
	: m_stream { stream }
	{
	}
	~ScopedTimer( )
	{
		m_end = Clock::now( );

		fmt::print( m_stream, "\nTimer took {}\n",
					std::chrono::duration_cast<Time>( m_end - m_start ) );
	}
	ScopedTimer( const ScopedTimer& ) = delete;
	ScopedTimer& operator=( const ScopedTimer& ) = delete;
};

}
