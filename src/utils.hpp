#pragma once

#include <algorithm>
#include <cstring>
#include <numeric>
#include <sstream>
#include <stack>
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

inline void set_value(char *strs[], const char *key, const char *new_value) {
  while (*strs != nullptr) {
    if (std::strcmp(*strs, key)) {
      std::strcpy(*strs, new_value);
      break;
    }
    strs++;
  }
}

inline std::string format_path(const std::string_view &path) {
  auto tokens = split(path, "/");
  std::stack<std::string> stk;
  for (const auto &token : tokens) {
    if (token == "..") {
      if (!stk.empty())
        stk.pop();
      continue;
    }
    if (!token.empty())
      stk.emplace(token);
  }
  std::vector<std::string> format_tokens;
  while (!stk.empty()) {
    format_tokens.emplace_back(stk.top());
    stk.pop();
  }
  std::reverse(format_tokens.begin(), format_tokens.end());
  if (format_tokens.empty())
    return "/";
  return std::accumulate(
      format_tokens.begin(), format_tokens.end(), std::string{},
      [](const std::string &a, const std::string &b) -> std::string {
        return a + "/" + b;
      });
}
} // namespace utils
