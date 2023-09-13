
#pragma once

#include <chrono>
#include <type_traits>
#include <functional>
#include <exception>
#include <utility>
#include <system_error>
#include <cstdio>


namespace simple_network_simulation::util
{

#define NODISCARD_WARNING_MSG "ignoring returned value of type 'ScopedTimer' might " \
                              "change the program's behavior since it has side effects"


template < class Duration = std::chrono::microseconds,
           class Clock    = std::chrono::steady_clock >
requires ( std::chrono::is_clock_v<Clock> &&
           requires { std::chrono::time_point<Clock, Duration>{ }; } )
class [[ nodiscard( NODISCARD_WARNING_MSG ) ]] ScopedTimer
{
public:
    using clock         = Clock;
    using duration      = Duration;
    using rep           = Duration::rep;
    using period        = Duration::period;
    using time_point    = std::chrono::time_point<Clock, Duration>;
    using callback_type = std::conditional_t<
                            noexcept( Clock::now( ) ),
                            std::move_only_function<void ( Duration ) noexcept>,
                            std::move_only_function<void ( Duration, std::exception_ptr ) noexcept> >;

    [[ nodiscard( "implicit destruction of temporary object" ) ]] explicit
    ScopedTimer( callback_type&& callback = nullptr ) noexcept( noexcept( now( ) ) )
        : m_callback { std::move( callback ) }
    {
    }

    [[ nodiscard( "implicit destruction of temporary object" ) ]]
    ScopedTimer( ScopedTimer&& rhs ) noexcept = default;

    ~ScopedTimer( )
    {
        if ( m_callback == nullptr )
            return;

        if constexpr ( noexcept( now( ) ) )
        {
            const time_point end { now( ) };
            m_callback( end - m_start );
        }
        else
        {
            try
            {
                const time_point end { now( ) };
                m_callback( end - m_start, nullptr );
            }
            catch ( ... )
            {
                const std::exception_ptr ex_ptr { std::current_exception( ) };
                m_callback( duration { }, ex_ptr );
            }
        }
    }

    [[ nodiscard ]] const time_point&
    get_start( ) const& noexcept
    {
        return m_start;
    }

    [[ nodiscard ]] const time_point&
    get_start( ) const&& noexcept = delete;

    [[ nodiscard ]] const callback_type&
    get_callback( ) const& noexcept
    {
        return m_callback;
    }

    [[ nodiscard ]] const callback_type&
    get_callback( ) const&& noexcept = delete;

    void
    set_callback( callback_type&& callback ) & noexcept
    {
        m_callback = std::move( callback );
    }

    void
    set_callback( callback_type&& callback ) && noexcept = delete;

    duration
    unset_callback( ) & noexcept( noexcept( elapsed_time( ) ) )
    {
        m_callback = nullptr;
        return elapsed_time( );
    }

    duration
    unset_callback( ) && noexcept( noexcept( elapsed_time( ) ) ) = delete;

    [[ nodiscard ]] duration
    elapsed_time( ) const& noexcept( noexcept( now( ) ) )
    {
        return now( ) - m_start;
    }

    [[ nodiscard ]] duration
    elapsed_time( ) const&& noexcept( noexcept( now( ) ) ) = delete;

    [[ nodiscard ]] time_point static
    now( ) noexcept( noexcept( clock::now( ) ) )
    {
        return std::chrono::time_point_cast<duration>( clock::now( ) );
    }

private:
    time_point const m_start { now( ) };
    callback_type m_callback;
};

template <class Callback>
ScopedTimer( Callback&& ) -> ScopedTimer<>;

template <class Duration>
ScopedTimer( std::move_only_function<void ( Duration ) noexcept> ) -> ScopedTimer<Duration>;


template <class Duration = std::chrono::microseconds>
requires ( requires { std::chrono::time_point<std::chrono::system_clock, Duration>{ }; } )
[[ nodiscard ]] auto inline
retrieve_current_local_time( )
{
    using namespace std::chrono;
    return zoned_time { current_zone( ), time_point_cast<Duration>( system_clock::now( ) ) };
}

void inline
flush_all_streams( ) noexcept( false )
{
    if ( std::fflush( nullptr ) ) [[ unlikely ]]
    {
        throw std::system_error { std::make_error_code( std::errc::io_error ),
                                  "Failure in flushing the streams" };
    }

    return;
}

void inline
flush_stderr( ) noexcept( false )
{
    if ( std::fflush( stderr ) ) [[ unlikely ]]
    {
        throw std::system_error { std::make_error_code( std::errc::io_error ),
                                  "Failure in flushing the stderr" };
    }

    return;
}

void inline
flush_stdout( ) noexcept( false )
{
    if ( std::fflush( stdout ) ) [[ unlikely ]]
    {
        throw std::system_error { std::make_error_code( std::errc::io_error ),
                                  "Failure in flushing the stdout" };
    }

    return;
}

}
