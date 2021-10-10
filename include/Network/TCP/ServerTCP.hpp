#pragma once

#include <asio.hpp>
#include <array>
#include <iostream>
#include <memory>

class session
{
private:
  asio::ip::tcp::socket& m_socket;
  enum {reading, writing } m_state = reading;
  std::array<char, 128> m_buffer;
  asio::const_buffer m_write_buffer;

public:
  session(asio::ip::tcp::socket& socket) : m_socket(socket){}

  bool want_read() const;
  void do_read(std::error_code& err);
  bool want_write() const;
  void do_write(std::error_code& err);
};

class connection : public std::enable_shared_from_this<connection>
{

private:
  asio::ip::tcp::socket m_socket;
  session m_session{m_socket};
  bool m_read_in_progress = false;
  bool m_write_in_progress = false;

  void do_operations();

public:
  connection(asio::ip::tcp::socket socket) : m_socket(std::move(socket)) {}

  void start();
};

class server
{
private:
  asio::ip::tcp::acceptor m_acceptor;

  void do_accept();

public:
  server(asio::io_context& io_context, unsigned short port) : m_acceptor(io_context, {asio::ip::tcp::v4(), port})
  {
    do_accept();
  }
};