#include <stdlib.h>
#include <iostream>


using namespace std;

string node1_process (string str);
string node2_process (string str);
string channel (string str);

int main ()
{
    string node1_message, nodes_1_to_2_channel_output, node2_message , nodes_2_to_1_channel_output = "0";
    while (nodes_2_to_1_channel_output != "")
    {
        node1_message = node1_process (nodes_2_to_1_channel_output);
        nodes_1_to_2_channel_output = channel (node1_message);
        node2_message = node2_process (nodes_1_to_2_channel_output);
        nodes_2_to_1_channel_output = channel (node2_message);
        cout << endl;
    }

    return 0;
}

string node1_process (string str)
{
    static int node1_request;
    string my_message_str;

    if (str != "0")
    {
        cout << "Node1_process received " << str << endl;
    }

    if (node1_request == 0)
    {
       my_message_str = "00000";
    }
    else if (node1_request == 1)
    {
        my_message_str = "00001";
    }
    else if (node1_request == 2)
    {
        my_message_str = "00010";
    }
    else if (node1_request == 3)
    {
        my_message_str = "00011";
    }
    else
    {
        my_message_str = "00111";
    }
    cout << "Node1_process is sending " << my_message_str << endl;
    node1_request++;
    return my_message_str;
}


string channel (string str)
{
    string my_str = str;
    cout << "Channel is sending " << str << endl;
    return my_str;
}

string node2_process (string str)
{
    cout << "Node2_process received "<< str << endl;
    string my_message_str;
    if (str == "00000")
    {
        my_message_str = "10000000";
    }
    else if (str == "00001")
    {
        my_message_str = "10000001";
    }
    else if (str == "00010")
    {
        my_message_str = "10000010";
    }
    else if (str == "00011")
    {
        my_message_str = "10000011";
    }
    else
    {
        my_message_str = "";
    }
    cout << "Node2_process is sending " << my_message_str << endl;


    return my_message_str;
}