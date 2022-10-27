#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace utils {
inline std::string strip(const std::string_view &inpt) {
  auto start_it = inpt.begin();
  auto end_it = inpt.rbegin();
  while (std::isspace(*start_it))
    ++start_it;
  while (std::isspace(*end_it))
    ++end_it;
  return std::string(start_it, end_it.base());
}

inline std::vector<std::string> split(const std::string_view &s,
                                      const std::string_view &delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;
  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.emplace_back(token);
  }
  res.emplace_back(s.substr(pos_start));
  return res;
}

inline std::vector<std::string> split_by_space(const std::string_view &s) {
  std::stringstream ss(std::string{s});
  std::vector<std::string> res;
  std::string token;
  while (ss >> token) {
    res.emplace_back(token);
  }
  return res;
}
} // namespace utils
