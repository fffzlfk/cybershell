//
// Created by fffzlfk on 22-10-27.
//
#pragma once

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string_view>
#include <unistd.h>
#include <vector>

#include "utils.hpp"

namespace fs = std::filesystem;
using namespace fs;

namespace cybershell {

// 从环境变量`PATH`里查找命令
inline std::optional<fs::path> find_file_in_path(const fs::path &filepath) {
  auto paths = utils::split(std::getenv("PATH"), ":");
  for (const auto &path : paths) {
    auto new_filepath = fs::path(path) / filepath;
    if (fs::exists(new_filepath)) {
      return new_filepath;
    }
  }
  return {};
}

// `cd`命令实现
inline bool cd(const std::string_view &target_path_str, char *envp[]) {
  auto target_path = fs::path{target_path_str};
  if (target_path.is_absolute()) {
    target_path = utils::format_path(target_path.c_str());
  } else if (target_path.is_relative())
    target_path = utils::format_path(
        (fs::path(std::getenv("PWD")) / target_path).c_str());
  if (!fs::is_directory(target_path)) {
    std::cout << target_path << " is not a directory.\n";
    return false;
  }
  if (chdir(target_path.c_str()) < 0) {
    std::cout << "permission denied.\n";
    return false;
  }
  const auto old_pwd = std::getenv("PWD");
  setenv("PWD", target_path.c_str(), 1);
  setenv("OLDPWD", old_pwd, 1);
  utils::set_value(envp, "PWD",
                   std::string{"PWD=" + target_path.string()}.c_str());
  utils::set_value(envp, "OLD_PWD", old_pwd);
  return true;
}

// 处理`cd`命令
inline bool solve_cd(const std::string_view &input, char *envp[]) {
  auto args = utils::split_by_space(input);
  if (args[0] == "cd") {
    if (args.size() == 2) {
      if (args[1] == "-") {
        args[1] = std::getenv("OLDPWD");
      } else if (utils::start_with(args[1], "~")) {
        args[1].replace(args[1].begin(), args[1].begin() + 1,
                        std::string{std::getenv("HOME")});
      }
      cd(args[1], envp);
    } else
      std::cout << "cd can only take one argument.\n";
    return true;
  }
  return false;
}

// 解析输入
inline std::optional<std::tuple<const char *, char **const, size_t>>
parse_input(std::string_view input) {
  auto it = input.find(' ');
  auto filepath = fs::path(input.substr(0, it));
  // absolute path
  if (filepath.is_absolute()) {
    if (!fs::exists(filepath)) {
      std::cout << filepath << " is not exists or not a file.\n";
      return {};
    }
  } else if (filepath.is_relative()) {
    // find in PATH
    if (filepath.filename() == filepath.string()) {
      auto filepath_in_path = find_file_in_path(filepath);
      if (!filepath_in_path.has_value()) {
        std::cout << filepath << " is not exists or not a file.\n";
        return {};
      }
      filepath = filepath_in_path.value();
    } else {
      // relative path
      auto absolute_path =
          utils::format_path((fs::path(std::getenv("PWD")) / filepath).c_str());
      if (!fs::exists(absolute_path)) {
        std::cout << absolute_path << " is not exists or not a file.\n";
        return {};
      }
      filepath = absolute_path;
    }
  }

  if ((fs::status(filepath).permissions() & fs::perms::owner_exec) ==
      fs::perms::none) {
    std::cout << "permission denied.\n";
    return {};
  }

  // 准备给`execve`传递的参数
  std::vector<std::string> args = utils::split_by_space(input);
  args[0] = filepath.string();
  auto ret_args = new char *[args.size() + 1];
  for (size_t i = 0; i < args.size(); i++) {
    ret_args[i] = new char[args[i].length() + 1];
    std::strcpy(ret_args[i], args[i].c_str());
  }
  ret_args[args.size()] = nullptr;
  auto ret_filepath = new char[filepath.string().length() + 1];
  std::strcpy(ret_filepath, filepath.c_str());
  return std::make_tuple(ret_filepath, ret_args, args.size());
}

// 处理退出命令
inline constexpr void solve_quit_command(const std::string_view &input) {
  constexpr std::array<const char *, 4> QUIT_COMMANDS = {"\\q", "q", "quit",
                                                         "exit"};
  for (const auto qc : QUIT_COMMANDS) {
    if (input == qc)
      exit(0);
  }
}

// 打印`prompt`
inline void print_prompt() {
  auto pwd = utils::format_path(std::getenv("PWD"));
  auto home = std::getenv("HOME");
  if (utils::start_with(pwd, home)) {
    utils::replace(pwd, home, "~");
  }
  std::cout << pwd << "> ";
}
} // namespace cybershell
