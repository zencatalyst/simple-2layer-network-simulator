
#include "BidirectionalMultimessageSimulation.hpp"
#include <iostream>
#include <iomanip>
#include <syncstream>
#include <string>
#include <sstream>
#include <string_view>
#include <span>
#include <utility>
#include <random>
#include <thread>
#include <cstddef>


using std::size_t;
using std::uint32_t;

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

	struct [[ nodiscard ]] UiStrings
	{
		std::string application_layer_text_head { ( std::ostringstream { } << std::setfill( '*' ) << std::setw( 24 )
																		   << "[Application Layer]"
																		   << std::setfill( '*' ) << std::setw( 53 )
																		   << "\n\n" ).str( ) };

		std::string application_layer_text_tail { ( std::ostringstream { } << '\n'
																		   << std::setfill( '*' ) << std::setw( 77 )
																		   << "\n\n" ).str( ) };

		std::string transport_layer_text_head { ( std::ostringstream { } << std::setfill( '-' ) << std::setw( 22 )
																		 << "[Transport Layer]"
																		 << std::setfill( '-' ) << std::setw( 55 )
																		 << "\n\n" ).str( ) };

		std::string transport_layer_text_tail { ( std::ostringstream { } << '\n'
																		 << std::setfill( '-' ) << std::setw( 77 )
																		 << "\n\n" ).str( ) };

		std::string channel_text_head { ( std::ostringstream { } << std::setfill( '~' ) << std::setw( 14 )
																 << "[Channel]"
																 << std::setfill( '~' ) << std::setw( 63 )
																 << "\n\n" ).str( ) };

		std::string channel_text_tail { ( std::ostringstream { } << '\n'
																 << std::setfill( '~' ) << std::setw( 77 )
																 << "\n\n" ).str( ) };
	};

	const UiStrings ui_strings { UiStrings { } };
}

std::random_device rand_dev { };


[[ nodiscard ]] payload_t node1_process1( const uint32_t process_num,
										  const std::pair<payload_t, bool>& incoming_payload,
										  std::osyncstream& out_sync_stream )
{
	payload_t payload;
	payload.m_source_port_num = process_num;

	const auto& [ received_payload, isIntact ] { incoming_payload };

	if ( isIntact )
	{
		out_sync_stream << ui_strings.application_layer_text_head
						<< "node1_process1 received message: <" << received_payload.m_message
						<< "> from source #" << received_payload.m_source_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );

		if ( received_payload.m_message == 0b1001'1111 ) [[ unlikely ]]
		{
			payload.m_destination_port_num = 0;
		}
		else
		{
			payload.m_destination_port_num = 7002;

			static size_t node1_process1_request_counter { };

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

		out_sync_stream << ui_strings.application_layer_text_head
						<< "node1_process1 is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );
	}
	else
	{
		out_sync_stream << ui_strings.application_layer_text_head
						<< "node1_process1 received corrupt message: <" << received_payload.m_message
						<< ">"
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );

		payload.m_destination_port_num = 0;

		std::this_thread::sleep_for( node1_process1_application_layer_delay );

		out_sync_stream << ui_strings.application_layer_text_head
						<< "node1_process1 is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );
	}

	return payload;
}

[[ nodiscard ]] payload_t node1_process2( const uint32_t process_num,
										  const std::pair<payload_t, bool>& incoming_payload,
										  std::osyncstream& out_sync_stream )
{
	payload_t payload;
	payload.m_source_port_num = process_num;

	const auto& [ received_payload, isIntact ] { incoming_payload };

	if ( isIntact )
	{
		out_sync_stream << ui_strings.application_layer_text_head
						<< "node1_process2 received message: <" << received_payload.m_message
						<< "> from source #" << received_payload.m_source_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );

		if ( received_payload.m_message == 0b1000'1111 ) [[ unlikely ]]
		{
			payload.m_destination_port_num = 0;
		}
		else
		{
			payload.m_destination_port_num = 7001;

			static size_t node1_process2_request_counter { };

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

		out_sync_stream << ui_strings.application_layer_text_head
						<< "node1_process2 is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );
	}
	else
	{
		out_sync_stream << ui_strings.application_layer_text_head
						<< "node1_process2 received corrupt message: <" << received_payload.m_message
						<< ">"
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );

		payload.m_destination_port_num = 0;

		std::this_thread::sleep_for( node1_process2_application_layer_delay );

		out_sync_stream << ui_strings.application_layer_text_head
						<< "node1_process2 is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );
	}

	return payload;
}

[[ nodiscard ]] payload_t node2_process1( const uint32_t process_num,
										  const std::pair<payload_t, bool>& incoming_payload,
										  std::osyncstream& out_sync_stream )
{
	payload_t payload;
	payload.m_source_port_num = process_num;

	const auto& [ received_payload, isIntact ] { incoming_payload };

	if ( isIntact )
	{
		out_sync_stream << ui_strings.application_layer_text_head
						<< "node2_process1 received message: <" << received_payload.m_message
						<< "> from source #" << received_payload.m_source_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );

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

		out_sync_stream << ui_strings.application_layer_text_head
						<< "node2_process1 is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );
	}
	else
	{
		out_sync_stream << ui_strings.application_layer_text_head
						<< "node2_process1 received corrupt message: <" << received_payload.m_message
						<< ">"
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );

		payload.m_destination_port_num = 0;

		std::this_thread::sleep_for( node2_process1_application_layer_delay );

		out_sync_stream << ui_strings.application_layer_text_head
						<< "node2_process1 is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );
	}

	return payload;
}

[[ nodiscard ]] payload_t node2_process2( const uint32_t process_num,
										  const std::pair<payload_t, bool>& incoming_payload,
										  std::osyncstream& out_sync_stream )
{
	payload_t payload;
	payload.m_source_port_num = process_num;

	const auto& [ received_payload, isIntact ] { incoming_payload };

	if ( isIntact )
	{
		out_sync_stream << ui_strings.application_layer_text_head
						<< "node2_process2 received message: <" << received_payload.m_message
						<< "> from source #" << received_payload.m_source_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );

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

		out_sync_stream << ui_strings.application_layer_text_head
						<< "node2_process2 is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );
	}
	else
	{
		out_sync_stream << ui_strings.application_layer_text_head
						<< "node2_process2 received corrupt message: <" << received_payload.m_message
						<< ">"
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );

		payload.m_destination_port_num = 0;

		std::this_thread::sleep_for( node2_process2_application_layer_delay );

		out_sync_stream << ui_strings.application_layer_text_head
						<< "node2_process2 is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.application_layer_text_tail;

		out_sync_stream.emit( );
	}

	return payload;
}

[[ nodiscard ]] segment_t channel( segment_t segment,
								   std::osyncstream& out_sync_stream )
{
	out_sync_stream << ui_strings.channel_text_head
					<< "channel received: <" << segment
					<< ">"
					<< '\n'
					<< ui_strings.channel_text_tail;

	out_sync_stream.emit( );

	static std::mt19937 mtgen { rand_dev( ) };
	static std::uniform_int_distribution uniform_50_50_dist { 1, 2 };

	if ( isChannelFaulty && uniform_50_50_dist( mtgen ) == 1 )
	{
		static std::uniform_int_distribution<size_t> uniform_dist_for_bit_select { 0, segment.size( ) - 1 };
		const auto random_index { uniform_dist_for_bit_select( mtgen ) };
		segment.flip( random_index );
	}

	std::this_thread::sleep_for( channel_delay );

	out_sync_stream << ui_strings.channel_text_head
					<< "channel is sending: <" << segment
					<< ">"
					<< '\n'
					<< ui_strings.channel_text_tail;

	out_sync_stream.emit( );

	return segment;
}

[[ nodiscard ]] segment_t node1_transport_to_channel( const payload_t payload,
													  std::osyncstream& out_sync_stream )
{
	out_sync_stream << ui_strings.transport_layer_text_head
					<< "node1_transport received message: <" << payload.m_message
					<< "> from source #" << payload.m_source_port_num
					<< '\n'
					<< ui_strings.transport_layer_text_tail;

	out_sync_stream.emit( );

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

	out_sync_stream << ui_strings.transport_layer_text_head
					<< "node1_transport is sending segment: <" << segment
					<< "> to destination #" << payload.m_destination_port_num
					<< '\n'
					<< ui_strings.transport_layer_text_tail;

	out_sync_stream.emit( );

	return segment;
}

[[ nodiscard ]] std::pair<payload_t, bool> node1_transport_from_channel( const segment_t segment,
																		 std::osyncstream& out_sync_stream )
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
		out_sync_stream << ui_strings.transport_layer_text_head
						<< "node1_transport received segment: <" << segment
						<< "> from source #" << payload.m_source_port_num
						<< '\n'
						<< ui_strings.transport_layer_text_tail;

		out_sync_stream.emit( );

		isIntact = true;

		std::this_thread::sleep_for( node1_transport_from_layer_delay );

		out_sync_stream << ui_strings.transport_layer_text_head
						<< "node1_transport is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.transport_layer_text_tail;

		out_sync_stream.emit( );
	}
	else
	{
		out_sync_stream << ui_strings.transport_layer_text_head
						<< "node1_transport received corrupt segment: <" << segment
						<< ">"
						<< '\n'
						<< ui_strings.transport_layer_text_tail;

		out_sync_stream.emit( );

		isIntact = false;

		std::this_thread::sleep_for( node1_transport_from_layer_delay );

		out_sync_stream << ui_strings.transport_layer_text_head
						<< "node1_transport is sending corrupt message: <" << payload.m_message
						<< ">"
						<< '\n'
						<< ui_strings.transport_layer_text_tail;

		out_sync_stream.emit( );
	}

	return result;
}

[[ nodiscard ]] segment_t node2_transport_to_channel( const payload_t payload,
													  std::osyncstream& out_sync_stream )
{
	out_sync_stream << ui_strings.transport_layer_text_head
					<< "node2_transport received message: <" << payload.m_message
					<< "> from source #" << payload.m_source_port_num
					<< '\n'
					<< ui_strings.transport_layer_text_tail;

	out_sync_stream.emit( );

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

	out_sync_stream << ui_strings.transport_layer_text_head
					<< "node2_transport is sending segment: <" << segment
					<< "> to destination #" << payload.m_destination_port_num
					<< '\n'
					<< ui_strings.transport_layer_text_tail;

	out_sync_stream.emit( );

	return segment;
}

[[ nodiscard ]] std::pair<payload_t, bool> node2_transport_from_channel( const segment_t segment,
																		 std::osyncstream& out_sync_stream )
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
		out_sync_stream << ui_strings.transport_layer_text_head
						<< "node2_transport received segment: <" << segment
						<< "> from source #" << payload.m_source_port_num
						<< '\n'
						<< ui_strings.transport_layer_text_tail;

		out_sync_stream.emit( );

		isIntact = true;

		std::this_thread::sleep_for( node2_transport_from_layer_delay );

		out_sync_stream << ui_strings.transport_layer_text_head
						<< "node2_transport is sending message: <" << payload.m_message
						<< "> to destination #" << payload.m_destination_port_num
						<< '\n'
						<< ui_strings.transport_layer_text_tail;

		out_sync_stream.emit( );
	}
	else
	{
		out_sync_stream << ui_strings.transport_layer_text_head
						<< "node2_transport received corrupt segment: <" << segment
						<< ">"
						<< '\n'
						<< ui_strings.transport_layer_text_tail;

		out_sync_stream.emit( );

		isIntact = false;

		std::this_thread::sleep_for( node2_transport_from_layer_delay );

		out_sync_stream << ui_strings.transport_layer_text_head
						<< "node2_transport is sending corrupt message: <" << payload.m_message
						<< ">"
						<< '\n'
						<< ui_strings.transport_layer_text_tail;

		out_sync_stream.emit( );
	}

	return result;
}

void execute_connection1( const uint32_t node1_process1_num, const uint32_t node2_process2_num )
{
	std::osyncstream out_sync_stream { std::cout };

	std::pair<payload_t, bool> node1_process1_payload_from_transport { payload_t { }, true };
	std::pair<payload_t, bool> node2_process2_payload_from_transport { payload_t { }, true };

	while ( true )
	{
		payload_t node1_process1_payload { node1_process1( node1_process1_num,
														   node1_process1_payload_from_transport,
														   out_sync_stream ) };

		if ( node1_process1_payload.m_destination_port_num == 0 )
		{
			out_sync_stream << R"(    /|\/|\/|\    closing connection1 by node1_process1...    /|\/|\/|\     )""\n\n";
			out_sync_stream.emit( );

			break;
		}

		segment_t node1_process1_segment { node1_transport_to_channel( node1_process1_payload,
																	   out_sync_stream ) };

		segment_t nodes_1_to_2_channel_output { channel( node1_process1_segment,
														 out_sync_stream ) };

		node2_process2_payload_from_transport = node2_transport_from_channel( nodes_1_to_2_channel_output,
																			  out_sync_stream );

		payload_t node2_process2_payload { node2_process2( node2_process2_num,
														   node2_process2_payload_from_transport,
														   out_sync_stream ) };

		if ( node2_process2_payload.m_destination_port_num == 0 )
		{
			out_sync_stream << R"(    /|\/|\/|\    closing connection1 by node2_process2...    /|\/|\/|\     )""\n\n";
			out_sync_stream.emit( );

			break;
		}

		segment_t node2_process2_segment { node2_transport_to_channel( node2_process2_payload,
																	   out_sync_stream ) };

		segment_t nodes_2_to_1_channel_output { channel( node2_process2_segment,
														 out_sync_stream ) };

		node1_process1_payload_from_transport = node1_transport_from_channel( nodes_2_to_1_channel_output,
																			  out_sync_stream );
	}
}

void execute_connection2( const uint32_t node1_process2_num, const uint32_t node2_process1_num )
{
	std::osyncstream out_sync_stream { std::cout };

	std::pair<payload_t, bool> node1_process2_payload_from_transport { payload_t { }, true };
	std::pair<payload_t, bool> node2_process1_payload_from_transport { payload_t { }, true };

	while ( true )
	{
		payload_t node1_process2_payload { node1_process2( node1_process2_num,
														   node1_process2_payload_from_transport,
														   out_sync_stream ) };

		if ( node1_process2_payload.m_destination_port_num == 0 )
		{
			out_sync_stream << R"(    /|\/|\/|\    closing connection2 by node1_process2...    /|\/|\/|\     )""\n\n";
			out_sync_stream.emit( );

			break;
		}

		segment_t node1_process2_segment { node1_transport_to_channel( node1_process2_payload,
																	   out_sync_stream ) };

		segment_t nodes_1_to_2_channel_output { channel( node1_process2_segment,
														 out_sync_stream ) };

		node2_process1_payload_from_transport = node2_transport_from_channel( nodes_1_to_2_channel_output,
																			  out_sync_stream );

		payload_t node2_process1_payload { node2_process1( node2_process1_num,
														   node2_process1_payload_from_transport,
														   out_sync_stream ) };

		if ( node2_process1_payload.m_destination_port_num == 0 )
		{
			out_sync_stream << R"(    /|\/|\/|\    closing connection2 by node2_process1...    /|\/|\/|\     )""\n\n";
			out_sync_stream.emit( );

			break;
		}

		segment_t node2_process1_segment { node2_transport_to_channel( node2_process1_payload,
																	   out_sync_stream ) };

		segment_t nodes_2_to_1_channel_output { channel( node2_process1_segment,
														 out_sync_stream ) };

		node1_process2_payload_from_transport = node1_transport_from_channel( nodes_2_to_1_channel_output,
																			  out_sync_stream );
	}
}

bool initialize_program( const std::span<const char* const> command_line_arguments )
{
	bool hasError { false }; 

	constexpr std::string_view layer_delays_on_arg { "-layer-delays=on" };
	constexpr std::string_view layer_delays_off_arg { "-layer-delays=off" };
	constexpr std::string_view faulty_channel_yes_arg { "-faulty-channel=yes" };
	constexpr std::string_view faulty_channel_no_arg { "-faulty-channel=no" };

	const auto command_line_options { command_line_arguments.subspan( 1 ) };

	for ( const auto option_ptr : command_line_options )
	{
		const std::string_view option { option_ptr };

		if ( option == layer_delays_on_arg )
		{
			node1_process1_application_layer_delay = node1_process1_application_layer_default_delay;
			node1_process2_application_layer_delay = node1_process2_application_layer_default_delay;
			node2_process1_application_layer_delay = node2_process1_application_layer_default_delay;
			node2_process2_application_layer_delay = node2_process2_application_layer_default_delay;
			node1_transport_to_layer_delay = node1_transport_to_layer_default_delay;
			node1_transport_from_layer_delay = node1_transport_from_layer_default_delay;
			node2_transport_to_layer_delay = node2_transport_to_layer_default_delay;
			node2_transport_from_layer_delay = node2_transport_from_layer_default_delay;
			channel_delay = channel_default_delay;
		}
		else if ( option == layer_delays_off_arg )
		{
			using std::chrono_literals::operator""ms;

			node1_process1_application_layer_delay = 0ms;
			node1_process2_application_layer_delay = 0ms;
			node2_process1_application_layer_delay = 0ms;
			node2_process2_application_layer_delay = 0ms;
			node1_transport_to_layer_delay = 0ms;
			node1_transport_from_layer_delay = 0ms;
			node2_transport_to_layer_delay = 0ms;
			node2_transport_from_layer_delay = 0ms;
			channel_delay = 0ms;
		}
		else if ( option == faulty_channel_yes_arg )
		{
			isChannelFaulty = true;
		}
		else if ( option == faulty_channel_no_arg )
		{
			isChannelFaulty = false;
		}
		else
		{
			hasError = true;
			std::cerr << "\nInvalid command-line argument\n\n";

			return hasError;
		}
	}

	return hasError;
}

}
