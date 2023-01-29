
#include "BidirectionalMultimessageSimulation.hpp"
#include <chrono>
#include <string_view>
#include <random>
#include <utility>
#include <thread>
#include <cstddef>
#include <cstdint>
#include <fmt/core.h>
#include "Formatters.hpp"


using std::uint8_t;
using std::uint32_t;
using std::size_t;

namespace simple_network_simulation
{

namespace
{

bool isChannelFaulty;

using std::chrono_literals::operator""ms;

constexpr auto node1_process1_application_layer_default_delay { 450ms };
constexpr auto node1_process2_application_layer_default_delay { 500ms };
constexpr auto node2_process1_application_layer_default_delay { 550ms };
constexpr auto node2_process2_application_layer_default_delay { 440ms };
constexpr auto node1_transport_to_layer_default_delay { 990ms };
constexpr auto node1_transport_from_layer_default_delay { 1110ms };
constexpr auto node2_transport_to_layer_default_delay { 1010ms };
constexpr auto node2_transport_from_layer_default_delay { 1070ms };
constexpr auto channel_default_delay { 1500ms };

constinit auto node1_process1_application_layer_delay { 0ms };
constinit auto node1_process2_application_layer_delay { 0ms };
constinit auto node2_process1_application_layer_delay { 0ms };
constinit auto node2_process2_application_layer_delay { 0ms };
constinit auto node1_transport_to_layer_delay { 0ms };
constinit auto node1_transport_from_layer_delay { 0ms };
constinit auto node2_transport_to_layer_delay { 0ms };
constinit auto node2_transport_from_layer_delay { 0ms };
constinit auto channel_delay { 0ms };

namespace ui_strings
{

constexpr std::string_view application_layer_text_head { "*****[Application Layer]****************"
														 "***********************************\n\n" };
constexpr std::string_view application_layer_text_tail { "****************************************"
														 "***********************************\n\n" };

constexpr std::string_view transport_layer_text_head { "-----[Transport Layer]------------------"
													   "-----------------------------------\n\n" };
constexpr std::string_view transport_layer_text_tail { "----------------------------------------"
													   "-----------------------------------\n\n" };

constexpr std::string_view channel_text_head { "~~~~~[Channel]~~~~~~~~~~~~~~~~~~~~~~~~~~"
											   "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" };
constexpr std::string_view channel_text_tail { "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
											   "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" };

}

}


[[ nodiscard ]] payload_t
node1_process1( const uint32_t process_num,
				const std::pair<payload_t, bool>& incoming_payload )
{
	payload_t payload;
	payload.m_source_port_num = process_num;

	const auto& [ received_payload, isIntact ] { incoming_payload };

	if ( isIntact )
	{
		fmt::print( "{0}node1_process1 received message: <{1}> from source #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					received_payload.m_message,
					received_payload.m_source_port_num,
					ui_strings::application_layer_text_tail );

		if ( received_payload.m_message == 0b1001'1111 ) [[ unlikely ]]
		{
			payload.m_destination_port_num = 0;
		}
		else
		{
			payload.m_destination_port_num = 7002;

			thread_local size_t node1_process1_request_counter { };

			switch ( node1_process1_request_counter )
			{
				case 0 :
					payload.m_message = 0b0000'0000;
					break;
				case 1 :
					payload.m_message = 0b0000'0001;
					break;
				case 2 :
					payload.m_message = 0b0000'0010;
					break;
				case 3 :
					payload.m_message = 0b0000'0011;
					break;
				default :
					payload.m_message = 0b0000'0111;
			}

			++node1_process1_request_counter;
		}

		std::this_thread::sleep_for( node1_process1_application_layer_delay );

		fmt::print( "{0}node1_process1 is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::application_layer_text_tail );
	}
	else
	{
		fmt::print( "{0}node1_process1 received corrupt message: <{1}>\n\n{2}",
					ui_strings::application_layer_text_head,
					received_payload.m_message,
					ui_strings::application_layer_text_tail );

		payload.m_destination_port_num = 0;

		std::this_thread::sleep_for( node1_process1_application_layer_delay );

		fmt::print( "{0}node1_process1 is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::application_layer_text_tail );
	}

	return payload;
}

[[ nodiscard ]] payload_t
node1_process2( const uint32_t process_num,
				const std::pair<payload_t, bool>& incoming_payload )
{
	payload_t payload;
	payload.m_source_port_num = process_num;

	const auto& [ received_payload, isIntact ] { incoming_payload };

	if ( isIntact )
	{
		fmt::print( "{0}node1_process2 received message: <{1}> from source #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					received_payload.m_message,
					received_payload.m_source_port_num,
					ui_strings::application_layer_text_tail );

		if ( received_payload.m_message == 0b1000'1111 ) [[ unlikely ]]
		{
			payload.m_destination_port_num = 0;
		}
		else
		{
			payload.m_destination_port_num = 7001;

			thread_local size_t node1_process2_request_counter { };

			switch ( node1_process2_request_counter )
			{
				case 0 :
					payload.m_message = 0b1010'1010;
					break;
				case 1 :
					payload.m_message = 0b1010'1011;
					break;
				case 2 :
					payload.m_message = 0b1010'1100;
					break;
				case 3 :
					payload.m_message = 0b1010'1101;
					break;
				default :
					payload.m_message = 0b1010'1111;
			}

			++node1_process2_request_counter;
		}

		std::this_thread::sleep_for( node1_process2_application_layer_delay );

		fmt::print( "{0}node1_process2 is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::application_layer_text_tail );
	}
	else
	{
		fmt::print( "{0}node1_process2 received corrupt message: <{1}>\n\n{2}",
					ui_strings::application_layer_text_head,
					received_payload.m_message,
					ui_strings::application_layer_text_tail );

		payload.m_destination_port_num = 0;

		std::this_thread::sleep_for( node1_process2_application_layer_delay );

		fmt::print( "{0}node1_process2 is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::application_layer_text_tail );
	}

	return payload;
}

[[ nodiscard ]] payload_t
node2_process1( const uint32_t process_num,
				const std::pair<payload_t, bool>& incoming_payload )
{
	payload_t payload;
	payload.m_source_port_num = process_num;

	const auto& [ received_payload, isIntact ] { incoming_payload };

	if ( isIntact )
	{
		fmt::print( "{0}node2_process1 received message: <{1}> from source #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					received_payload.m_message,
					received_payload.m_source_port_num,
					ui_strings::application_layer_text_tail );

		payload.m_destination_port_num = 5002;

		switch ( received_payload.m_message.to_ullong( ) )
		{
			case 0b1010'1010 :
				payload.m_message = 0b0100'0000;
				break;
			case 0b1010'1011 :
				payload.m_message = 0b1000'0001;
				break;
			case 0b1010'1100 :
				payload.m_message = 0b1100'0010;
				break;
			case 0b1010'1101 :
				payload.m_message = 0b1110'0011;
				break;
			default :
				payload.m_message = 0b1000'1111;
		}

		std::this_thread::sleep_for( node2_process1_application_layer_delay );

		fmt::print( "{0}node2_process1 is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::application_layer_text_tail );
	}
	else
	{
		fmt::print( "{0}node2_process1 received corrupt message: <{1}>\n\n{2}",
					ui_strings::application_layer_text_head,
					received_payload.m_message,
					ui_strings::application_layer_text_tail );

		payload.m_destination_port_num = 0;

		std::this_thread::sleep_for( node2_process1_application_layer_delay );

		fmt::print( "{0}node2_process1 is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::application_layer_text_tail );
	}

	return payload;
}

[[ nodiscard ]] payload_t
node2_process2( const uint32_t process_num,
				const std::pair<payload_t, bool>& incoming_payload )
{
	payload_t payload;
	payload.m_source_port_num = process_num;

	const auto& [ received_payload, isIntact ] { incoming_payload };

	if ( isIntact )
	{
		fmt::print( "{0}node2_process2 received message: <{1}> from source #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					received_payload.m_message,
					received_payload.m_source_port_num,
					ui_strings::application_layer_text_tail );

		payload.m_destination_port_num = 5001;

		switch ( received_payload.m_message.to_ullong( ) )
		{
			case 0b0000'0000 :
				payload.m_message = 0b1001'1000;
				break;
			case 0b0000'0001 :
				payload.m_message = 0b1010'1000;
				break;
			case 0b0000'0010 :
				payload.m_message = 0b1011'1000;
				break;
			case 0b0000'0011 :
				payload.m_message = 0b1111'1000;
				break;
			default :
				payload.m_message = 0b1001'1111;
		}

		std::this_thread::sleep_for( node2_process2_application_layer_delay );

		fmt::print( "{0}node2_process2 is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::application_layer_text_tail );
	}
	else
	{
		fmt::print( "{0}node2_process2 received corrupt message: <{1}>\n\n{2}",
					ui_strings::application_layer_text_head,
					received_payload.m_message,
					ui_strings::application_layer_text_tail );

		payload.m_destination_port_num = 0;

		std::this_thread::sleep_for( node2_process2_application_layer_delay );

		fmt::print( "{0}node2_process2 is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::application_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::application_layer_text_tail );
	}

	return payload;
}

[[ nodiscard ]] segment_t
channel( segment_t segment )
{
	fmt::print( "{0}channel received: <{1}>\n\n{2}",
				ui_strings::channel_text_head,
				segment,
				ui_strings::channel_text_tail );

	thread_local std::random_device rand_dev { };
	thread_local std::mt19937 mtgen { rand_dev( ) };
	thread_local std::uniform_int_distribution<uint8_t> uniform_50_50_dist { 1, 2 };
	thread_local std::uniform_int_distribution<size_t> uniform_dist_for_bit_select { 0, segment.size( ) - 1 };

	if ( isChannelFaulty && uniform_50_50_dist( mtgen ) == 1 )
	{
		const auto random_index { uniform_dist_for_bit_select( mtgen ) };
		segment.flip( random_index );
	}

	std::this_thread::sleep_for( channel_delay );

	fmt::print( "{0}channel is sending: <{1}>\n\n{2}",
				ui_strings::channel_text_head,
				segment,
				ui_strings::channel_text_tail );

	return segment;
}

[[ nodiscard ]] segment_t
node1_transport_to_channel( const payload_t payload )
{
	fmt::print( "{0}node1_transport received message: <{1}> from source #{2}\n\n{3}",
				ui_strings::transport_layer_text_head,
				payload.m_message,
				payload.m_source_port_num,
				ui_strings::transport_layer_text_tail );

	segment_t segment { };
	segment |= segment_t { payload.m_message.to_ullong( ) };

	switch ( payload.m_source_port_num )
	{
		default :
			[[ fallthrough ]];
		case 5001 :
			segment[ 9 ] = false;
			break;
		case 5002 :
			segment[ 9 ] = true;
			break;
	}

	switch ( payload.m_destination_port_num )
	{
		default :
			[[ fallthrough ]];
		case 7001 :
			segment[ 8 ] = false;
			break;
		case 7002 :
			segment[ 8 ] = true;
			break;
	}

	if ( segment.count( ) % 2 == 0 )
	{
		segment[ 10 ] = false;
	}
	else
	{
		segment[ 10 ] = true;
	}

	std::this_thread::sleep_for( node1_transport_to_layer_delay );

	fmt::print( "{0}node1_transport is sending segment: <{1}> to destination #{2}\n\n{3}",
				ui_strings::transport_layer_text_head,
				segment,
				payload.m_destination_port_num,
				ui_strings::transport_layer_text_tail );

	return segment;
}

[[ nodiscard ]] std::pair<payload_t, bool>
node1_transport_from_channel( const segment_t segment )
{
	std::pair<payload_t, bool> result { };
	auto& [ payload, isIntact ] { result };

	payload.m_message |= message_t { segment.to_ullong( ) };

	if ( segment[ 9 ] == false )
	{
		payload.m_source_port_num = 7001;
	}
	else
	{
		payload.m_source_port_num = 7002;
	}

	if ( segment[ 8 ] == false )
	{
		payload.m_destination_port_num = 5001;
	}
	else
	{
		payload.m_destination_port_num = 5002;
	}

	if ( segment.count( ) % 2 == 0 )
	{
		fmt::print( "{0}node1_transport received segment: <{1}> from source #{2}\n\n{3}",
					ui_strings::transport_layer_text_head,
					segment,
					payload.m_source_port_num,
					ui_strings::transport_layer_text_tail );

		isIntact = true;

		std::this_thread::sleep_for( node1_transport_from_layer_delay );

		fmt::print( "{0}node1_transport is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::transport_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::transport_layer_text_tail );
	}
	else
	{
		fmt::print( "{0}node1_transport received corrupt segment: <{1}>\n\n{2}",
					ui_strings::transport_layer_text_head,
					segment,
					ui_strings::transport_layer_text_tail );

		isIntact = false;

		std::this_thread::sleep_for( node1_transport_from_layer_delay );

		fmt::print( "{0}node1_transport is sending corrupt message: <{1}>\n\n{2}",
					ui_strings::transport_layer_text_head,
					payload.m_message,
					ui_strings::transport_layer_text_tail );
	}

	return result;
}

[[ nodiscard ]] segment_t
node2_transport_to_channel( const payload_t payload )
{
	fmt::print( "{0}node2_transport received message: <{1}> from source #{2}\n\n{3}",
				ui_strings::transport_layer_text_head,
				payload.m_message,
				payload.m_source_port_num,
				ui_strings::transport_layer_text_tail );

	segment_t segment { };
	segment |= segment_t { payload.m_message.to_ullong( ) };

	switch ( payload.m_source_port_num )
	{
		default :
			[[ fallthrough ]];
		case 7001 :
			segment[ 9 ] = false;
			break;
		case 7002 :
			segment[ 9 ] = true;
			break;
	}

	switch ( payload.m_destination_port_num )
	{
		default :
			[[ fallthrough ]];
		case 5001 :
			segment[ 8 ] = false;
			break;
		case 5002 :
			segment[ 8 ] = true;
			break;
	}

	if ( segment.count( ) % 2 == 0 )
	{
		segment[ 10 ] = false;
	}
	else
	{
		segment[ 10 ] = true;
	}

	std::this_thread::sleep_for( node2_transport_to_layer_delay );

	fmt::print( "{0}node2_transport is sending segment: <{1}> to destination #{2}\n\n{3}",
				ui_strings::transport_layer_text_head,
				segment,
				payload.m_destination_port_num,
				ui_strings::transport_layer_text_tail );

	return segment;
}

[[ nodiscard ]] std::pair<payload_t, bool>
node2_transport_from_channel( const segment_t segment )
{
	std::pair<payload_t, bool> result { };
	auto& [ payload, isIntact ] { result };

	payload.m_message |= message_t { segment.to_ullong( ) };

	if ( segment[ 9 ] == false )
	{
		payload.m_source_port_num = 5001;
	}
	else
	{
		payload.m_source_port_num = 5002;
	}

	if ( segment[ 8 ] == false )
	{
		payload.m_destination_port_num = 7001;
	}
	else
	{
		payload.m_destination_port_num = 7002;
	}

	if ( segment.count( ) % 2 == 0 )
	{
		fmt::print( "{0}node2_transport received segment: <{1}> from source #{2}\n\n{3}",
					ui_strings::transport_layer_text_head,
					segment,
					payload.m_source_port_num,
					ui_strings::transport_layer_text_tail );

		isIntact = true;

		std::this_thread::sleep_for( node2_transport_from_layer_delay );

		fmt::print( "{0}node2_transport is sending message: <{1}> to destination #{2}\n\n{3}",
					ui_strings::transport_layer_text_head,
					payload.m_message,
					payload.m_destination_port_num,
					ui_strings::transport_layer_text_tail );
	}
	else
	{
		fmt::print( "{0}node2_transport received corrupt segment: <{1}>\n\n{2}",
					ui_strings::transport_layer_text_head,
					segment,
					ui_strings::transport_layer_text_tail );

		isIntact = false;

		std::this_thread::sleep_for( node2_transport_from_layer_delay );

		fmt::print( "{0}node2_transport is sending corrupt message: <{1}>\n\n{2}",
					ui_strings::transport_layer_text_head,
					payload.m_message,
					ui_strings::transport_layer_text_tail );
	}

	return result;
}

void
execute_connection1( const uint32_t node1_process1_num,
					 const uint32_t node2_process2_num )
{
	std::pair<payload_t, bool> node1_process1_payload_from_transport { payload_t { }, true };
	std::pair<payload_t, bool> node2_process2_payload_from_transport { payload_t { }, true };

	while ( true )
	{
		payload_t node1_process1_payload { node1_process1( node1_process1_num,
														   node1_process1_payload_from_transport ) };

		if ( node1_process1_payload.m_destination_port_num == 0 )
		{
			fmt::print( R"(    /|\/|\/|\    closing connection1 by node1_process1...    /|\/|\/|\     )""\n\n" );

			break;
		}

		segment_t node1_process1_segment { node1_transport_to_channel( node1_process1_payload ) };

		segment_t nodes_1_to_2_channel_output { channel( node1_process1_segment ) };

		node2_process2_payload_from_transport = node2_transport_from_channel( nodes_1_to_2_channel_output );

		payload_t node2_process2_payload { node2_process2( node2_process2_num,
														   node2_process2_payload_from_transport ) };

		if ( node2_process2_payload.m_destination_port_num == 0 )
		{
			fmt::print( R"(    /|\/|\/|\    closing connection1 by node2_process2...    /|\/|\/|\     )""\n\n" );

			break;
		}

		segment_t node2_process2_segment { node2_transport_to_channel( node2_process2_payload ) };

		segment_t nodes_2_to_1_channel_output { channel( node2_process2_segment ) };

		node1_process1_payload_from_transport = node1_transport_from_channel( nodes_2_to_1_channel_output );
	}
}

void
execute_connection2( const uint32_t node1_process2_num,
					 const uint32_t node2_process1_num )
{
	std::pair<payload_t, bool> node1_process2_payload_from_transport { payload_t { }, true };
	std::pair<payload_t, bool> node2_process1_payload_from_transport { payload_t { }, true };

	while ( true )
	{
		payload_t node1_process2_payload { node1_process2( node1_process2_num,
														   node1_process2_payload_from_transport ) };

		if ( node1_process2_payload.m_destination_port_num == 0 )
		{
			fmt::print( R"(    /|\/|\/|\    closing connection2 by node1_process2...    /|\/|\/|\     )""\n\n" );

			break;
		}

		segment_t node1_process2_segment { node1_transport_to_channel( node1_process2_payload ) };

		segment_t nodes_1_to_2_channel_output { channel( node1_process2_segment ) };

		node2_process1_payload_from_transport = node2_transport_from_channel( nodes_1_to_2_channel_output );

		payload_t node2_process1_payload { node2_process1( node2_process1_num,
														   node2_process1_payload_from_transport ) };

		if ( node2_process1_payload.m_destination_port_num == 0 )
		{
			fmt::print( R"(    /|\/|\/|\    closing connection2 by node2_process1...    /|\/|\/|\     )""\n\n" );

			break;
		}

		segment_t node2_process1_segment { node2_transport_to_channel( node2_process1_payload ) };

		segment_t nodes_2_to_1_channel_output { channel( node2_process1_segment ) };

		node1_process2_payload_from_transport = node1_transport_from_channel( nodes_2_to_1_channel_output );
	}
}

void
set_layers_delays( const bool layers_delays_status ) noexcept
{
	if ( layers_delays_status == true )
	{
		node1_process1_application_layer_delay = node1_process1_application_layer_default_delay;
		node1_process2_application_layer_delay = node1_process2_application_layer_default_delay;
		node2_process1_application_layer_delay = node2_process1_application_layer_default_delay;
		node2_process2_application_layer_delay = node2_process2_application_layer_default_delay;
		node1_transport_to_layer_delay         = node1_transport_to_layer_default_delay;
		node1_transport_from_layer_delay       = node1_transport_from_layer_default_delay;
		node2_transport_to_layer_delay         = node2_transport_to_layer_default_delay;
		node2_transport_from_layer_delay       = node2_transport_from_layer_default_delay;
		channel_delay                          = channel_default_delay;
	}
	else
	{
		using std::chrono_literals::operator""ms;

		node1_process1_application_layer_delay = 0ms;
		node1_process2_application_layer_delay = 0ms;
		node2_process1_application_layer_delay = 0ms;
		node2_process2_application_layer_delay = 0ms;
		node1_transport_to_layer_delay         = 0ms;
		node1_transport_from_layer_delay       = 0ms;
		node2_transport_to_layer_delay         = 0ms;
		node2_transport_from_layer_delay       = 0ms;
		channel_delay                          = 0ms;
	}
}

void
set_channel_faults( const bool channel_faults_status ) noexcept
{
	if ( channel_faults_status == true )
	{
		isChannelFaulty = true;
	}
	else
	{
		isChannelFaulty = false;
	}
}

}
