
#pragma once

#include <bitset>
#include <utility>
#include <cstddef>
#include <cstdint>


namespace simple_network_simulation
{

inline constexpr auto parity_bit_count               { 1uz };
inline constexpr auto source_port_num_bit_count      { 1uz };
inline constexpr auto destination_port_num_bit_count { 1uz };
inline constexpr auto payload_bit_count              { 8uz };
inline constexpr auto segment_bit_count              { parity_bit_count + source_port_num_bit_count +
                                                       destination_port_num_bit_count + payload_bit_count };

struct [[ nodiscard ]] payload_t
{
    std::bitset< payload_bit_count > data;
};

struct [[ nodiscard ]] message_t
{
    payload_t payload;
    std::uint32_t source_port_num;
    std::uint32_t destination_port_num;
};

struct [[ nodiscard ]] segment_t
{
    std::bitset< segment_bit_count > data;
};

[[ nodiscard ]] message_t
node1_process1( const std::uint32_t process_num,
                const std::pair<message_t, bool>& incoming_message );

[[ nodiscard ]] message_t
node1_process2( const std::uint32_t process_num,
                const std::pair<message_t, bool>& incoming_message );

[[ nodiscard ]] message_t
node2_process1( const std::uint32_t process_num,
                const std::pair<message_t, bool>& incoming_message );

[[ nodiscard ]] message_t
node2_process2( const std::uint32_t process_num,
                const std::pair<message_t, bool>& incoming_message );

[[ nodiscard ]] segment_t
channel( segment_t segment );

[[ nodiscard ]] segment_t
node1_transport_to_channel( const message_t message );

[[ nodiscard ]] std::pair<message_t, bool>
node1_transport_from_channel( const segment_t segment );

[[ nodiscard ]] segment_t
node2_transport_to_channel( const message_t message );

[[ nodiscard ]] std::pair<message_t, bool>
node2_transport_from_channel( const segment_t segment );

void
execute_connection1( const std::uint32_t node1_process1_num,
                     const std::uint32_t node2_process2_num );

void
execute_connection2( const std::uint32_t node1_process2_num,
                     const std::uint32_t node2_process1_num );

}
