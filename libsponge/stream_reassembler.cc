#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), _unassembled_bytes(0), _unassemabled_strs() {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    DUMMY_CODE(data, index, eof);
    // 1.如果index大于等于_output.write_index()+capacity()，则直接丢弃
    if (index >= _output.bytes_written() + _capacity)
        return;
    // 2.如果index+data.size()小于_output.write_index()，则直接丢弃
    if (index + data.size() <= _output.bytes_written())
        return;
    // 3.如果index小于等于_output.write_index()，则需要截取data
    if (index <= _output.bytes_written()) {
        // 如果index+data.size()大于等于first_unacceptable_index(),则需要截取data,并将_unassemabled_strs清空
        size_t start = _output.bytes_written() - index;
        size_t unacceptable_index = first_unacceptable_index();
        if (index + data.size() >= unacceptable_index) {
            size_t len = unacceptable_index - _output.bytes_written();
            _unassemabled_strs.clear();
            _unassembled_bytes = 0;
            _unassemabled_strs[index + start] = data.substr(start, len);
            _unassembled_bytes += len;
            _output.write(_unassemabled_strs[index + start]);
            _unassemabled_strs.clear();
            _unassembled_bytes = 0;
        } else {
            // 如果_unassemabled_strs没有元素，则直接写入
            if (_unassemabled_strs.empty()) {
                _output.write(data.substr(start));
                return;
            }
            // 找到_unassemabled_strs中第一个大于等于index+data.size()的元素,如果没有则返回最后一个元素
            auto it = _unassemabled_strs.lower_bound(index + data.size());
            if (it == _unassemabled_strs.end()) {
                // 如果没有则返回最后一个元素
                it = _unassemabled_strs.end();
                it--;
                // 如果index+data.size()大于等于最后一个元素的index+data.size(),则直接写入
                if (index + data.size() >= it->first + it->second.size()) {
                    _output.write(data.substr(start));
                    _unassemabled_strs.clear();
                    _unassembled_bytes += 0;
                    return;
                }
                // 如果index+data.size()小于最后一个元素的index+data.size(),则需要截取data
                size_t to_write_len = it->first - _output.bytes_written();
                _output.write(data.substr(start, to_write_len));
                _output.write(it->second);
                _unassemabled_strs.clear();
                _unassembled_bytes = 0;
            }
            // 存在大于等于index+data.size()的元素
            else {
                // 如果index+data.size()等于it->first，则拼接后直接写入
                if (index + data.size() == it->first) {
                    _output.write(data.substr(start) + it->second);
                    _unassemabled_strs.erase(it);
                    _unassembled_bytes -= it->second.size();
                    return;
                }
                // 如果index+data.size()大于it->first，如果it有上一个元素，则拿到上一个元素查看和data的重叠情况
                if (it != _unassemabled_strs.begin()) {
                    auto pre_it = it;
                    pre_it--;
                    // 如果index+data.size()大于等于pre_it->first+pre_it->second.size(),则拼接后直接写入
                    if (index + data.size() >= pre_it->first + pre_it->second.size()) {
                        _output.write(data.substr(start) + it->second);
                        _unassemabled_strs.erase(it);
                        _unassembled_bytes -= it->second.size();
                        return;
                    }
                    // 如果index+data.size()小于pre_it->first+pre_it->second.size(),则需要截取data
                    size_t to_write_len = pre_it->first + pre_it->second.size() - _output.bytes_written();
                    _output.write(data.substr(start, to_write_len));
                    _output.write(pre_it->second);
                    _unassemabled_strs.erase(pre_it);
                    _unassembled_bytes -= pre_it->second.size();
                    return;
                }
            }
        }

    } else {
        size_t len = min(data.size(), _capacity - _unassembled_bytes);
        _unassemabled_strs[index] = data.substr(0, len);
        _unassembled_bytes += len;
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_bytes; }

bool StreamReassembler::empty() const { return _unassembled_bytes == 0; }
size_t StreamReassembler::first_unacceptable_index() const { return _output.bytes_read() + _capacity; }