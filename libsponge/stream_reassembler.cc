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
    if (index >= _output.bytes_written() + _capacity) {
        if (eof) {
            _eof_flag = true;
            _eof_idx = index + data.size();
        }

        if (_eof_flag && _eof_idx <= _output.bytes_written())
            _output.end_input();
        return;
    }

    // 2.如果index+data.size()小于_output.write_index()，则直接丢弃
    if (index + data.size() <= _output.bytes_written()) {
        if (eof) {
            _eof_flag = true;
            _eof_idx = index + data.size();
        }

        if (_eof_flag && _eof_idx <= _output.bytes_written())
            _output.end_input();
        return;
    }

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
                if (eof) {
                    _eof_flag = true;
                    _eof_idx = index + data.size();
                }

                if (_eof_flag && _eof_idx <= _output.bytes_written())
                    _output.end_input();
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
                    if (eof) {
                        _eof_flag = true;
                        _eof_idx = index + data.size();
                    }

                    if (_eof_flag && _eof_idx <= _output.bytes_written())
                        _output.end_input();
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
                    if (eof) {
                        _eof_flag = true;
                        _eof_idx = index + data.size();
                    }

                    if (_eof_flag && _eof_idx <= _output.bytes_written())
                        _output.end_input();
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
                        if (eof) {
                            _eof_flag = true;
                            _eof_idx = index + data.size();
                        }

                        if (_eof_flag && _eof_idx <= _output.bytes_written())
                            _output.end_input();
                        return;
                    }
                    // 如果index+data.size()小于pre_it->first+pre_it->second.size(),则需要截取data
                    size_t to_write_len = pre_it->first + pre_it->second.size() - _output.bytes_written();
                    _output.write(data.substr(start, to_write_len));
                    _output.write(pre_it->second);
                    _unassemabled_strs.erase(pre_it);
                    _unassembled_bytes -= pre_it->second.size();
                    if (eof) {
                        _eof_flag = true;
                        _eof_idx = index + data.size();
                    }

                    if (_eof_flag && _eof_idx <= _output.bytes_written())
                        _output.end_input();
                    return;
                }
                // 如果it没有上一个元素，直接将data写入
                else {
                    _output.write(data.substr(start));
                    if (eof) {
                        _eof_flag = true;
                        _eof_idx = index + data.size();
                    }

                    if (_eof_flag && _eof_idx <= _output.bytes_written())
                        _output.end_input();
                    return;
                }
            }
        }

    }
    // 处理index大于_output.write_index()的情况
    else {
        // 如果index+data.size()大于等于first_unacceptable_index(),则需要截取data
        size_t unacceptable_index = first_unacceptable_index();
        if (index + data.size() >= unacceptable_index) {
            size_t len = unacceptable_index - index;
            // 将与data重叠的元素删除,并将拼接好的新串加入到_unassemabled_strs中
            auto it = _unassemabled_strs.lower_bound(index);
            if (it != _unassemabled_strs.end()) {
                auto pre_it = it;
                pre_it--;
                if (pre_it->first + pre_it->second.size() > index) {
                    // 去掉重复部分，拼接data和pre_it->second，得到一个新字符串
                    string new_str = data.substr(0, len) + pre_it->second.substr(pre_it->first, index - pre_it->first);
                    // 使用循环去除_unassemabled_strs中pre_it以及之后的元素
                    while (pre_it != _unassemabled_strs.end()) {
                        _unassembled_bytes -= pre_it->second.size();
                        pre_it = _unassemabled_strs.erase(pre_it);
                    }
                    // 将新字符串加入到_unassemabled_strs中
                    _unassemabled_strs[pre_it->first] = new_str;
                    _unassembled_bytes += new_str.size();
                }
                if (eof) {
                    _eof_flag = true;
                    _eof_idx = index + data.size();
                }

                if (_eof_flag && _eof_idx <= _output.bytes_written())
                    _output.end_input();
                return;
            } else {
                // 去除it以及之后的元素
                while (it != _unassemabled_strs.end()) {
                    _unassembled_bytes -= it->second.size();
                    it = _unassemabled_strs.erase(it);
                }
                // 将新字符串加入到_unassemabled_strs中
                _unassemabled_strs[index] = data.substr(0, len);
                _unassembled_bytes += len;
                if (eof) {
                    _eof_flag = true;
                    _eof_idx = index + data.size();
                }

                if (_eof_flag && _eof_idx <= _output.bytes_written())
                    _output.end_input();
                return;
            }
        }
        // 如果index+data.size()小于first_unacceptable_index(),判断data首尾是否与_unassemabled_strs中的元素重叠
        else {
            // 如果_unassemabled_strs为空，则直接写入
            if (_unassemabled_strs.empty()) {
                // 将data写入_unassemabled_strs
                _unassemabled_strs[index] = data;
                _unassembled_bytes += data.size();
                if (eof) {
                    _eof_flag = true;
                    _eof_idx = index + data.size();
                }

                if (_eof_flag && _eof_idx <= _output.bytes_written())
                    _output.end_input();
                return;
            }
            // 找到_unassemabled_strs中第一个大于等于index的元素,如果没有则返回第一个元素
            auto it = _unassemabled_strs.lower_bound(index);
            // 没有找到第一个大于等于index的元素，拿到第一个小于index的元素
            if (it == _unassemabled_strs.end()) {
                // 拿到第一个小于index的元素
                it--;
                // 如果it包含data,则直接返回
                if (it->first + it->second.size() >= index + data.size()) {
                    if (eof) {
                        _eof_flag = true;
                        _eof_idx = index + data.size();
                    }

                    if (_eof_flag && _eof_idx <= _output.bytes_written())
                        _output.end_input();
                    return;
                }
                // 如果index大于第一个元素的index+data.size(),则直接写入
                if (index > it->first + it->second.size()) {
                    // 将data写入_unassemabled_strs
                    _unassemabled_strs[index] = data;
                    _unassembled_bytes += data.size();
                    if (eof) {
                        _eof_flag = true;
                        _eof_idx = index + data.size();
                    }

                    if (_eof_flag && _eof_idx <= _output.bytes_written())
                        _output.end_input();
                    return;
                }
                // 如果index小于等于第一个元素的index+data.size(),则需要截取data
                else {
                    size_t overlap_len = it->first + it->second.size() - index;
                    // 将与data重叠的元素删除,并将拼接好的新串加入到_unassemabled_strs中
                    string new_str = it->second + data.substr(index + overlap_len);
                    // 将新字符串加入到_unassemabled_strs中
                    _unassemabled_strs[it->first] = new_str;
                    _unassembled_bytes += data.size() - overlap_len;
                    if (eof) {
                        _eof_flag = true;
                        _eof_idx = index + data.size();
                    }

                    if (_eof_flag && _eof_idx <= _output.bytes_written())
                        _output.end_input();
                    return;
                }
            }
            // 存在第一个大于等于index的元素,这种情况需要同时处理首部和尾部重叠
            else {
                auto temp_len = data.size();
                auto temp_str = data;
                auto temp = index;
                //  申明flag,初始化为it
                auto flag_start = it;
                // 拿到第一个大于等于index的元素，判断他是否为_unassebled_strs的第一个元素
                // 处理首部重叠
                if (it != _unassemabled_strs.begin()) {
                    // 将it复制给pre_it
                    auto pre_it = it;
                    pre_it--;
                    // 如果index大于pre_it->first+pre_it->second.size()
                    if (index > pre_it->first + pre_it->second.size()) {
                        // 将data加入到_unassemabled_strs中
                        _unassemabled_strs[index] = data;
                        // 将it复制给flag
                        flag_start = it;
                    }
                    // 如果index小于等于pre_it->first+pre_it->second.size()，则需要截取data
                    else {
                        // 如果pre_it包含data，则直接返回
                        if (pre_it->first + pre_it->second.size() >= index + data.size()) {
                            if (eof) {
                                _eof_flag = true;
                                _eof_idx = index + data.size();
                            }

                            if (_eof_flag && _eof_idx <= _output.bytes_written())
                                _output.end_input();
                            return;
                        }
                        // size_t overlap_len = pre_it->first + pre_it->second.size() - index;
                        // 将与data重叠的元素删除,并将拼接好的新串加入到_unassemabled_strs中
                        temp_str += pre_it->second.substr(pre_it->first, index - pre_it->first);  // 不会越界
                        // 将新串加入到_unassemabled_strs中
                        _unassemabled_strs[pre_it->first] = temp_str;
                        _unassembled_bytes -= pre_it->second.size();
                        temp = pre_it->first;
                        temp_len += index - pre_it->first;
                        flag_start = it;
                    }
                } else {
                    flag_start = it;
                    _unassemabled_strs[temp] = data;
                }
                // 拿到_unassemabled_strs中第一个大于index+data.size()的元素
                auto end_it = _unassemabled_strs.upper_bound(index + data.size());
                auto flag_end = end_it;
                // 将it复制给pre_it
                auto pre_it = end_it;
                pre_it--;
                // 如果index+data.size()大于pre_it->first+pre_it->second.size()
                if (index + data.size() > pre_it->first + pre_it->second.size()) {
                    // 将end_it复制给flag
                    flag_end = end_it;
                }
                // 如果index+data.size()小于等于pre_it->first+pre_it->second.size()，则需要截取data
                else {
                    // size_t overlap_len = index + data.size() - pre_it->first;
                    // 将与data重叠的元素删除,并将拼接好的新串加入到_unassemabled_strs中
                    temp_str += pre_it->second.substr(index + data.size(),
                                                      pre_it->first + pre_it->second.size() - index -
                                                          data.size());  // 这里不会越界
                    // 将新串加入到_unassemabled_strs中
                    _unassemabled_strs[temp] = temp_str;
                    _unassembled_bytes -= index + data.size() - pre_it->first;
                    temp_len += pre_it->first + pre_it->second.size() - index - data.size();
                    flag_end = end_it;
                }
                //_unassemabled_strs中存在大于index+data.size()的元素
                // else {
                //     _unassemabled_strs[temp] = temp_str;
                //     flag_end = end_it;
                // }
                // 将temp_len加入到_unassembled_bytes中
                _unassembled_bytes += temp_len;
                // 将flag_start到flag_end之间的元素删除,erase是前闭后开的
                auto earse_end = ++flag_end;
                _unassemabled_strs.erase(flag_start, earse_end);
            }
        }
    }
    if (eof) {
        _eof_flag = true;
        _eof_idx = index + data.size();
    }

    if (_eof_flag && _eof_idx <= _output.bytes_written())
        _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_bytes; }
bool StreamReassembler::empty() const { return _unassembled_bytes == 0; }
size_t StreamReassembler::first_unacceptable_index() const { return _output.bytes_read() + _capacity; }