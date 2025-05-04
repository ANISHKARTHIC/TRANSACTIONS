# Bank Transactions (C Project)

This is a simple terminal-based program in C to manage basic bank account operations using binary files. It's a menu-driven app that lets you add, update, delete, and view account details. All data is stored in a file called `credit.dat`, and there's an option to export account info to a readable text file.

## What It Does

- Add a new account with name and balance
- Update the balance of an existing account
- Delete an account
- View all accounts in a formatted `.txt` file

## Files in This Project

- `trans.c` – main C file with all the logic
- `credit.dat` – binary file where the accounts are stored
- `accounts.txt` – generated file with all account info (good for printing)
- `trans.exe` – compiled version for Windows
- `clients.dat` – not currently used, might be leftover/test file

## How to Compile and Run

To compile:

```bash
gcc trans.c -o trans.exe
```

To run:

```bash
./trans.exe
```

You’ll get a simple menu in the terminal where you can pick what you want to do.

## FEW DETAILS

- Account numbers go from 1 to 100
- Each account has: account number, first name, last name, and balance
- If you try to add an account that already exists, it won’t overwrite it
- Deleted accounts are just cleared in the file
- Works on Windows and Linux (tested with GCC)
