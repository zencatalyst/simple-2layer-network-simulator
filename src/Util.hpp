
#pragma once

#include <chrono>
#include <type_traits>
#include <functional>
#include <exception>
#include <utility>


namespace simple_network_simulation::util
{

template < class Duration = std::chrono::microseconds,
		   class Clock    = std::chrono::steady_clock >
requires ( std::chrono::is_clock_v<Clock> &&
		   requires { std::chrono::time_point<Clock, Duration>{ }; } )
struct [[ nodiscard ]] ScopedTimer
{
	using clock         = Clock;
	using duration      = Duration;
	using rep           = Duration::rep;
	using period        = Duration::period;
	using time_point    = std::chrono::time_point<Clock, Duration>;
	using callback_type = std::conditional_t< noexcept( Clock::now( ) ),
											  std::move_only_function<void ( Duration ) noexcept>,
											  std::move_only_function<void ( Duration, std::exception_ptr ) noexcept> >;

	time_point const start { now( ) };
	callback_type callback;

	explicit ScopedTimer( callback_type&& call_back = nullptr ) noexcept( noexcept( now( ) ) )
		: callback { std::move( call_back ) }
	{
	}

	ScopedTimer( ScopedTimer&& rhs ) noexcept = default;

	~ScopedTimer( )
	{
		if ( callback == nullptr )
			return;

		if constexpr ( noexcept( now( ) ) )
		{
			const time_point end { now( ) };
			callback( end - start );
		}
		else
		{
			try
			{
				const time_point end { now( ) };
				callback( end - start, nullptr );
			}
			catch ( ... )
			{
				const std::exception_ptr ex_ptr { std::current_exception( ) };
				callback( duration { }, ex_ptr );
			}
		}
	}

	[[ nodiscard ]] duration
	elapsed_time( ) const& noexcept( noexcept( now( ) ) )
	{
		return now( ) - start;
	}

	[[ nodiscard ]] duration
	elapsed_time( ) const&& noexcept( noexcept( now( ) ) ) = delete;

	[[ nodiscard ]] time_point static
	now( ) noexcept( noexcept( clock::now( ) ) )
	{
		return std::chrono::time_point_cast<duration>( clock::now( ) );
	}
};

template <class Callback>
ScopedTimer( Callback ) -> ScopedTimer<>;

}
