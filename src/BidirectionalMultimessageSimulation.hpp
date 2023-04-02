
#pragma once

#include <bitset>
#include <utility>
#include <cstddef>
#include <cstdint>


namespace simple_network_simulation
{

inline constexpr std::size_t parity_bit_count { 1 };
inline constexpr std::size_t source_port_num_bit_count { 1 };
inline constexpr std::size_t destination_port_num_bit_count { 1 };
inline constexpr std::size_t message_bit_count { 8 };
inline constexpr std::size_t segment_bit_count { parity_bit_count + source_port_num_bit_count +
                                                 destination_port_num_bit_count + message_bit_count };

using message_t = std::bitset< message_bit_count >;
using segment_t = std::bitset< segment_bit_count >;

struct payload_t
{
    message_t m_message;
    std::uint32_t m_source_port_num;
    std::uint32_t m_destination_port_num;
};


[[ nodiscard ]] payload_t
node1_process1( const std::uint32_t process_num,
                const std::pair<payload_t, bool>& incoming_payload );

[[ nodiscard ]] payload_t
node1_process2( const std::uint32_t process_num,
                const std::pair<payload_t, bool>& incoming_payload );

[[ nodiscard ]] payload_t
node2_process1( const std::uint32_t process_num,
                const std::pair<payload_t, bool>& incoming_payload );

[[ nodiscard ]] payload_t
node2_process2( const std::uint32_t process_num,
                const std::pair<payload_t, bool>& incoming_payload );

[[ nodiscard ]] segment_t
channel( segment_t segment );

[[ nodiscard ]] segment_t
node1_transport_to_channel( const payload_t payload );

[[ nodiscard ]] std::pair<payload_t, bool>
node1_transport_from_channel( const segment_t segment );

[[ nodiscard ]] segment_t
node2_transport_to_channel( const payload_t payload );

[[ nodiscard ]] std::pair<payload_t, bool>
node2_transport_from_channel( const segment_t segment );

void
execute_connection1( const std::uint32_t node1_process1_num,
                     const std::uint32_t node2_process2_num );

void
execute_connection2( const std::uint32_t node1_process2_num,
                     const std::uint32_t node2_process1_num );

}
