//
// Created by fffzlfk on 22-10-27.
//

#include <sys/wait.h>
#include <unistd.h>

#include "lib.hpp"

int main(int __argc, char *__argv[], char *envp[]) {
  using namespace cybershell;
  {
    (void)__argc;
    (void)__argv;
  }
  while (true) {
    print_prompt();
    std::string input{};
    std::getline(std::cin, input);
    input = utils::strip(input);

    if (input.empty())
      continue;

    solve_quit_command(input);

    if (solve_cd(input, envp))
      continue;

    auto parsed_res = parse_input(input);
    if (!parsed_res.has_value()) {
      continue;
    }
    auto [filepath, args, argc] = parsed_res.value();

    pid_t pid = fork(); // fork出子进程
    if (pid < 0) {
      // fork出错
      std::cout << "fork error.\n";
      continue;
    } else if (pid == 0) {
      // 子进程

      // 改变工作目录为环境变量`PWD`，即`cd`之后的目录
      if (chdir(std::getenv("PWD")) < 0) {
        std::cout << "chdir error.\n";
        exit(-1);
      }

      // 使用`execve`将当前子进程替换为执行`filepath`，其中参数`envp`是环境变量
      if (execve(filepath, args, envp) < 0) {
        std::cout << "exec error.\n";
      }
    } else {
      // 父进程
      int status = 0;
      wait(&status); // 阻塞等待子进程结束

      // 子进程异常退出
      if (!WIFEXITED(status)) {
        printf("child process error: %d.\n", WEXITSTATUS(status));
      }
    }

    // 回收内存
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
