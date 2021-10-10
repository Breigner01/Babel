#include "ServerTCP.hpp"

// Session class declaration

bool session::want_read() const
{
    return m_state == reading;
}

void session::do_read(std::error_code& err)
{
  if (std::size_t len = m_socket.read_some(asio::buffer(m_buffer), err)) {
    m_write_buffer = asio::buffer(m_buffer, len);
    m_state = writing;
  }
}

bool session::want_write() const
{
  return m_state == writing;
}

void session::do_write(std::error_code& err)
{
  if (std::size_t len = m_socket.write_some(asio::buffer(m_write_buffer), err)) {
    m_write_buffer = m_write_buffer + len;
    m_state = asio::buffer_size(m_write_buffer) > 0 ? writing : reading;
  }
}

// End of session class declaratioon

// connection class declaration

// public

void connection::start()
{
  m_socket.non_blocking(true);
  do_operations();
}

// private

void connection::do_operations()
{
  auto self(shared_from_this());
  if (m_session.want_read() && !m_read_in_progress) {
    m_read_in_progress = true;
    m_socket.async_wait(asio::ip::tcp::socket::wait_read,
        [this, self](std::error_code err)
        {
          m_read_in_progress = false;
          if (!err)
            m_session.do_read(err);
          if (!err || err == asio::error::would_block)
            do_operations();
          else
            m_socket.close();
        });
  }

  if (m_session.want_write() && !m_write_in_progress) {
    m_write_in_progress = true;
    m_socket.async_wait(asio::ip::tcp::socket::wait_write,
        [this, self](std::error_code err)
        {
          m_write_in_progress = false;
          if (!err)
            m_session.do_write(err);
          if (!err || err == asio::error::would_block)
            do_operations();
          else
            m_socket.close();
        });
  }
}

// End of connection class declaration


// server class declaration

void server::do_accept()
{
  m_acceptor.async_accept(
      [this](std::error_code err, asio::ip::tcp::socket socket) {
        if (!err) {
          std::make_shared<connection>(std::move(socket))->start();
        }

        do_accept();
      });
}

// End of server class declaration
