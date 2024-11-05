#include <iostream>
// #include <windows.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool setExecutePermission(const char *filepath)
{
  // Set execute permissions: owner, group, others (rwxr-xr-x)
  if (chmod(filepath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1)
  {
    std::cerr << "Error setting execute permission on " << filepath << ": " << strerror(errno) << '\n';
    return false;
  }
  return true;
}

int startup()
{
  const char *filepath = "./bin/game";

  if (!setExecutePermission(filepath))
  {
    std::cerr << "Unable to set execute permissions." << std::endl;
    return 1;
  }

  char *command[] = {strdup(filepath), nullptr};

  pid_t pid = fork(); // Create a new process

  if (pid == -1)
  {
    std::cerr << "[Debug] Failed to fork()\n";
    return 1;
  }
  else if (pid > 0)
  {
    // Parent process
    int status;
    waitpid(pid, &status, 0); // Wait for the child process to finish
    if (WIFEXITED(status))
    {
      std::cout << "[Debug] Child exited with status " << WEXITSTATUS(status) << "\n";
    }
  }
  else
  {
    // Child process
    execvp(command[0], command);
    // If execvp returns, an error occurred
    std::cerr << "[Debug] Failed to execvp " << command[0] << ": " << strerror(errno) << "\n";
    free(command[0]); // Free the duplicated string
    _exit(1);         // Exit child process
  }

  // Free the memory in the parent process
  free(command[0]);
  return 0;
}

int main()
{
  if (startup() == 1)
  {
    std::cout << "Error while starting up!\n";
    return 1;
  }
  return 0;
}