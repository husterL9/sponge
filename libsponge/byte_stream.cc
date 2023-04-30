#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _capacity(capacity), _bytes_written(0), _bytes_read(0), _buffer({}) {
    DUMMY_CODE(capacity);
}

size_t ByteStream::write(const string &data) {
    // 错误示范
    //  if (data.size() > remaining_capacity()) {
    //      _buffer.insert(_buffer.end(), data.begin(), data.begin() + remaining_capacity());
    //      _bytes_written += remaining_capacity();
    //      return remaining_capacity();
    //  } else {
    //      _buffer.insert(_buffer.end(), data.begin(), data.end());
    //      _bytes_written += data.size();
    //      return data.size();
    //  }
    DUMMY_CODE(data);
    size_t len = remaining_capacity();
    if (data.size() > len) {
        _buffer.insert(_buffer.end(), data.begin(), data.begin() + len);
        _bytes_written += len;
        return len;
    } else {
        _buffer.insert(_buffer.end(), data.begin(), data.end());
        _bytes_written += data.size();
        return data.size();
    }
    // size_t delta_len = std::min(data.size(), _capacity - _buffer.size());
    // _buffer.insert(_buffer.end(), data.begin(), data.begin() + delta_len);
    // _bytes_written += delta_len;
    // return delta_len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    DUMMY_CODE(len);
    // copy the first len bytes of the buffer to a string
    string result;
    if (len > _buffer.size()) {
        result = string(_buffer.begin(), _buffer.end());
    } else {
        result = string(_buffer.begin(), _buffer.begin() + len);
    }
    return result;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    DUMMY_CODE(len);
    if (len > _buffer.size()) {
        _buffer.clear();
        _bytes_read += _buffer.size();
    } else {
        _buffer.erase(_buffer.begin(), _buffer.begin() + len);
        _bytes_read += len;
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    DUMMY_CODE(len);
    string result = peek_output(len);
    pop_output(len);
    return result;
}

void ByteStream::end_input() { _input_ended = true; }

bool ByteStream::input_ended() const { return _input_ended; }

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return _buffer.empty(); }

bool ByteStream::eof() const { return input_ended() && _buffer.empty(); }  //

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - _buffer.size(); }
