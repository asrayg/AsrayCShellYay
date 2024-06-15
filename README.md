## Asray's C Shell

### Overview

This project is a simple shell implementation in C, developed as a learning exercise to understand various concepts in system programming, including system calls, process management, file handling, and signal handling.

### Features

- **Basic Command Execution**: Execute basic shell commands by typing them in the prompt.
- **Built-in Commands**: Includes a set of built-in commands such as `cd`, `help`, `exit`, `setenv`, `unsetenv`, `alias`, `unalias`, and more.
- **Piping**: Supports command piping using the `|` operator, allowing the output of one command to be used as input for another.
- **Input/Output Redirection**: Handles input and output redirection using `>` and `<` operators.
- **Subshell Execution**: Allows execution of commands within a subshell using parentheses.
- **Job Control**: Supports job control, including background execution and job listing.
- **Command History**: Maintains a history of executed commands, which can be viewed using the `history_list` command.
- **Alias Management**: Allows creating and managing command aliases.
- **Environment Variables**: Supports setting and unsetting environment variables.
- **Prompt Customization**: Allows customization of the shell prompt.
- **Directory Stack**: Supports directory stack operations with `pushd`, `popd`, and `dirs`.

### File Structure

```plaintext
.
├── main.c
├── shell.h
├── builtins.c
├── builtins.h
├── utils.c
├── utils.h
├── job_control.c
├── job_control.h
├── readline.c
├── readline.h
└── Makefile
```

### Compilation

To compile the project, use the provided `Makefile`. Simply run:

```sh
make
```

This will generate an executable named `shell`.

### Usage

To run the shell, execute the compiled `shell` binary:

```sh
./shell
```

You will be greeted with a prompt where you can start typing commands.

### Built-in Commands

The shell includes the following built-in commands:

- `cd <directory>`: Change the current directory.
- `help`: Display the help message with the list of built-in commands.
- `exit`: Exit the shell.
- `setenv <variable> <value>`: Set an environment variable.
- `unsetenv <variable>`: Unset an environment variable.
- `alias <name>=<command>`: Create an alias for a command.
- `unalias <name>`: Remove an alias.
- `cat <file>`: Display the contents of a file.
- `ls <directory>`: List the contents of a directory.
- `cp <source> <destination>`: Copy a file.
- `mv <source> <destination>`: Move or rename a file.
- `rm <file>`: Remove a file.
- `pushd <directory>`: Push the current directory onto the stack and change to a new directory.
- `popd`: Pop the top directory off the stack and change to it.
- `dirs`: Display the directory stack.
- `source <file>`: Read and execute commands from a file.
- `setprompt <prompt>`: Change the shell prompt.
- `calc <expression>`: Evaluate a mathematical expression.
- `set <variable> <value>`: Set a shell variable.
- `get <variable>`: Get the value of a shell variable.
- `history_list`: Display the command history.
- `which <command>`: Locate a command.
- `echo <message>`: Display a message.
- `fg <job>`: Bring a background job to the foreground.
- `bg <job>`: Resume a stopped job in the background.

### Learning Objectives

This project was designed to provide hands-on experience with:

- System calls and process management in Unix-like operating systems.
- Handling signals and implementing job control.
- Understanding file I/O operations and redirection.
- Building a command-line interpreter and understanding shell internals.
- Implementing custom built-in commands and managing command history.
- Using libraries like `readline` for enhanced user interaction.

### Acknowledgements

This project was developed as part of a learning exercise to gain practical experience with system programming concepts. It draws inspiration from various online resources and tutorials on shell implementation and system programming in C.

### License

This project is open-source and available under the MIT License. Feel free to use, modify, and distribute the code as needed.

---

Feel free to update and modify the README file to better suit your project's needs and any additional features you may add in the future.