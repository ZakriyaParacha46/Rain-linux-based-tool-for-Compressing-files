# Rain File Archiver

This project is inspired by: [assignment specs.pdf](https://github.com/ZakriyaParacha46/Rain-linux-based-tool-for-Compressing-files/files/11584434/assignment.2.specs.pdf)

Rain is a file archiver tool written in C that allows you to create, list, check, and extract files from a custom file format called "drop." It supports various functionalities, including handling file permissions, hashing for integrity checks, and working with different storage formats.

![image](https://github.com/ZakriyaParacha46/Rain-linux-based-tool-for-Compressing-files/assets/82748498/58fba438-20dd-4bd3-a218-6495dae7a68c)

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

## Reference Implementation

A reference implementation, `1521 rain`, is provided as a common, efficient, and effective method to define an operational specification. Use it to find correct outputs and behaviors for any input:

```bash
$ 1521 rain -L examples/tiny.6-bit.drop
-rw-r--r-- 6 0 a
```

Discovering and matching the reference implementation's behavior is deliberately a part of this assignment. Report bugs in the class forum.

## Droplet Content Encodings

- **8-bit Format (Subset 3 only):** Contents are an array of bytes, equivalent to the bytes in the original file.
- **7-bit Format (Subset 3 only):** Contents are an array of bytes representing packed seven-bit values.
- **6-bit Format (Subset 3 only):** Contents are an array of bytes of packed six-bit values.

## Packed n-bit Encoding (Subset 3 only)

Values are stored inside larger types. For example, the integer 42 only needs six bits, so only those bits are stored.

## Droplet Hash

Each droplet ends with a hash calculated from the other values of the droplet. The `droplet_hash()` function is used for this calculation.

## Assumptions and Clarifications
- Make as few assumptions as possible. If in doubt, match the output of the reference implementation.
- Submitted code must be a single C program only.
- Use C standard library functions available on CSE Linux systems.
- Do not require extra compile options. Compile with `dcc *.c -o rain`.
- Do not use functions from other libraries.
- Disable debugging output before submission.
- Do not create or use temporary files.
- Do not create subprocesses.
