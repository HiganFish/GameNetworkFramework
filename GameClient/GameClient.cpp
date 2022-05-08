// GameClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <asio.hpp>

void TestSend()
{
    char TEST_DATA[] = { 0x02, 0x00, 0x00, 0x00, 0x48, 0x49 };
    asio::io_context context;
    asio::ip::tcp::socket socket(context);
    asio::ip::tcp::resolver resolver(context);
    asio::connect(socket, resolver.resolve("127.0.0.1", "40000"));

    for (int i = 0; i < 10; ++i)
    {
        asio::write(socket, asio::buffer(TEST_DATA, sizeof(TEST_DATA)));
    }
}

int main()
{

}
