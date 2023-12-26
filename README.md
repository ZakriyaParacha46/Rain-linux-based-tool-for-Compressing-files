# Rain File Archiver

Rain is a file archiver tool written in C that allows you to create, list, check, and extract files from a custom file format called "drop." It supports various functionalities, including handling file permissions, hashing for integrity checks, and working with different storage formats.

## Table of Contents

- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
  - [List Contents](#list-contents)
  - [Detailed List Contents](#detailed-list-contents)
  - [Check Drop](#check-drop)
  - [Extract Drop](#extract-drop)
  - [Create Drop](#create-drop)
- [Subsets](#subsets)
  - [Subset 0](#subset-0)
  - [Subset 1](#subset-1)
  - [Subset 2](#subset-2)
  - [Subset 3](#subset-3)
- [Contributing](#contributing)
- [License](#license)

## Getting Started

### Prerequisites

- C Compiler (e.g., GCC)
- Make (optional but recommended)

### Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/your-username/rain.git
   cd rain
   ```

2. Compile the code:

   ```bash
   make
   ```

   If you don't have `make`, you can use the alternative compilation commands mentioned in the assignment.

## Usage

### List Contents

```bash
./rain -l example.drop
```

This command will print a list of files in the drop.

### Detailed List Contents

```bash
./rain -L example.drop
```

This command will print detailed information about each file in the drop.

### Check Drop

```bash
./rain -C example.drop
```

This command will check the integrity of the drop, including hash checks and droplet magic numbers.

### Extract Drop

```bash
./rain -x example.drop
```

This command will extract the contents of the drop.

### Create Drop

```bash
./rain -c new.drop file1.txt file2.txt
```

This command will create a new drop containing the specified files.

## Subsets

### Subset 0

Subset 0 focuses on listing and providing detailed information about drop contents.

### Subset 1

Subset 1 adds functionality to check drop integrity and extract files.

### Subset 2

Subset 2 introduces the ability to create a drop from a list of files.

### Subset 3

Subset 3 extends the functionality to work with files and directories, including different storage formats.

## Contributing

Feel free to contribute to this project. Please follow the [Contributing Guidelines](CONTRIBUTING.md).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
```

This is a basic template, and you may want to customize it further based on the specific details of your project. Make sure to include a `LICENSE` file and, if applicable, a `CONTRIBUTING.md` file for guidelines on contributing to your project.



This project is inspired by: [assignment 2 specs.pdf](https://github.com/ZakriyaParacha46/Rain-linux-based-tool-for-Compressing-files/files/11584434/assignment.2.specs.pdf)
![image](https://github.com/ZakriyaParacha46/Rain-linux-based-tool-for-Compressing-files/assets/82748498/58fba438-20dd-4bd3-a218-6495dae7a68c)
