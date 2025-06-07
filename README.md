# Bank Account Management System (Windows GUI)

A secure, user-friendly banking application with a Windows GUI interface, built using the Win32 API. The application allows customers to manage their bank accounts with an intuitive interface, featuring secure PIN-based authentication and data persistence.

## ✨ Features

- **Modern Windows GUI** with intuitive dialogs and controls
- **Secure Account Management**
  - Register new accounts with personal details and 4-digit PIN
  - Secure login with account number and PIN
  - Change account PIN
  - Soft-delete accounts
- **Financial Operations**
  - Deposit funds
  - Withdraw funds (with balance validation)
  - View current balance
- **Data Management**
  - Export all accounts to a formatted text file
  - Persistent storage in binary format
  - Automatic data file creation if not exists

## 📁 Project Structure

| File                | Purpose                                                                 |
|---------------------|-------------------------------------------------------------------------|
| `trans.c`           | Main application source code with all business logic and GUI handlers   |
| `main_gui.rc`       | Resource script containing dialog box definitions                       |
| `resource.h`        | Header file with resource IDs and constants                            |
| `credit.dat`        | Binary database file storing all account information                   |
| `accounts.txt`      | Exported account information in readable format                        |
| `trans.exe`         | Compiled executable (Windows)                                          |
| `main_gui_res.o`    | Compiled resources (generated during build)                            |


## 🛠️ Prerequisites

- Windows OS
- MinGW-w64 (for compilation)
- Basic command-line knowledge

## 🔧 Compilation Instructions

1. **Install MinGW-w64** (if not already installed)
   - Download from: https://www.mingw-w64.org/
   - Add MinGW `bin` directory to your system PATH

2. **Compile the Resources**
   ```bash
   windres main_gui.rc -o main_gui_res.o
   ```

3. **Compile the Application**
   ```bash
   gcc trans.c main_gui_res.o -o trans.exe -mwindows -lcomctl32
   ```

## 🚀 Running the Application

1. Double-click `trans.exe` or run from command line:
   ```bash
   .\trans.exe
   ```

2. **Main Window**
   - Register: Create a new bank account
   - Login: Access existing account
   - Export: Save account data to `accounts.txt`
   - Exit: Close the application

## 💻 Usage Guide

### Creating a New Account
1. Click "Register"
2. Fill in your first name, last name, and a 4-digit PIN
3. Note your account number shown in the success message

### Accessing Your Account
1. Click "Login"
2. Enter your account number and PIN
3. Use the account management interface to perform transactions

### Managing Your Account
- **Deposit/Withdraw**: Enter amount and confirm
- **Change PIN**: Enter current and new PIN
- **Delete Account**: Requires account PIN for confirmation
- **Logout**: Returns to main menu

## 🔒 Security Notes

- All PINs are stored securely in the database
- Account numbers range from 100 to 999
- Data is automatically saved after each operation
- Deleted accounts are marked as inactive but remain in the database

## 📜 License

This project is open-source and available under the MIT License.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
- **PIN:** Must be exactly 4 digits
- **Deleted accounts:** Marked as inactive and can be reused
- **Cross-platform:** Works on Windows and Linux (tested with GCC)
- **Security:** PIN is required for all sensitive operations

## License

This project is for educational purposes.
