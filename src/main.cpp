//
// Created by fffzlfk on 22-10-27.
//
#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string_view>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "utils.hpp"

namespace fs = std::filesystem;
using namespace fs;

inline std::optional<fs::path> find_file_in_env(const fs::path &filepath) {
  auto paths = utils::split(secure_getenv("PATH"), ":");
  for (const auto &path : paths) {
    auto new_filepath = fs::path(path) / filepath;
    if (fs::is_regular_file(new_filepath)) {
      return new_filepath;
    }
  }
  return {};
}

std::optional<std::tuple<const char *, char **const, size_t>>
parse_input(std::string_view input) {
  auto it = input.find(' ');
  auto filepath = fs::path(input.substr(0, it));
  if (!fs::is_regular_file(filepath)) {
    auto new_filepath = find_file_in_env(filepath);
    if (!new_filepath.has_value()) {
      std::cout << filepath << " is not exists or not a file.\n";
      return {};
    }
    filepath = new_filepath.value();
  }

  if ((fs::status(filepath).permissions() & fs::perms::owner_exec) ==
      fs::perms::none) {
    std::cout << "permission denied.\n";
    return {};
  }

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

constexpr bool is_quit_command(const std::string_view &input) {
  constexpr std::array<const char *, 4> QUIT_COMMANDS = {"\\q", "q", "quit",
                                                         "exit"};
  for (const auto qc : QUIT_COMMANDS) {
    if (input == qc)
      return true;
  }
  return false;
}

int main(int __argc, char *__argv[], char *envp[]) {
  {
    (void)__argc;
    (void)__argv;
  }
  while (true) {
    std::cout << "cybershell> ";
    std::string input{};
    std::getline(std::cin, input);
    input = utils::strip(input);
    if (input.empty())
      continue;
    if (is_quit_command(input))
      exit(0);

    auto parsed_res = parse_input(input);
    if (!parsed_res.has_value()) {
      continue;
    }
    auto [filepath, args, argc] = parsed_res.value();
    pid_t pid = fork();
    if (pid < 0) {
      std::cout << "fork error.\n";
      continue;
    } else if (pid == 0) {
      if (execve(filepath, args, envp) < 0) {
        std::cout << "exec error.\n";
      }
    } else {
      int status = 0;
      wait(&status); // wait the end of child process
      if (!WIFEXITED(status)) {
        printf("child process error: %d.\n", WEXITSTATUS(status));
      }
    }
    for (size_t i = 0; i < argc; i++) {
      delete[] args[i];
    }
    delete[] filepath;
    delete[] args;
    if (pid == 0)
      exit(0);
  }
  return 0;
}