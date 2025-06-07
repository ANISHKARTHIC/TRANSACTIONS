#define WIN32_LEAN_AND_MEAN
#define _WIN32_IE 0x0500
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include "resource.h"

#pragma comment(lib, "comctl32.lib")

const char g_szClassName[] = "BankAppWindowClass";

#define MIN_ACCOUNT 100
#define MAX_ACCOUNT 999

#define MIN_ACCOUNT 100
#define MAX_ACCOUNT 999

struct clientData {
    unsigned int acctNum; // account number (3 digits)
    char lastName[15];
    char firstName[10];
    double balance;
    char pin[5];          // 4-digit PIN
    int isActive;         // 1 = active, 0 = deleted
};

// Find an available account number
int findAvailableAccount(FILE *fPtr) {
    struct clientData client;
    rewind(fPtr);
    for (int i = MIN_ACCOUNT; i <= MAX_ACCOUNT; ++i) {
        fread(&client, sizeof(client), 1, fPtr);
        if (client.acctNum == 0) return i;
    }
    return -1;
}

INT_PTR CALLBACK RegisterDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            char fname[16] = "", lname[16] = "", pin[8] = "";
            GetDlgItemText(hDlg, EDIT_FIRSTNAME, fname, sizeof(fname));
            GetDlgItemText(hDlg, EDIT_LASTNAME, lname, sizeof(lname));
            GetDlgItemText(hDlg, EDIT_PIN, pin, sizeof(pin));
            if (strlen(fname) == 0 || strlen(lname) == 0 || strlen(pin) != 4) {
                MessageBox(hDlg, "Please enter all fields. PIN must be 4 digits.", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            FILE *fPtr = fopen("credit.dat", "rb+");
            if (!fPtr) {
                // File does not exist, create and initialize
                fPtr = fopen("credit.dat", "wb+");
                if (!fPtr) {
                    MessageBox(hDlg, "Could not create data file.", "Error", MB_OK | MB_ICONERROR);
                    return TRUE;
                }
                struct clientData blankClient = {0, "", "", 0.0, "", 0};
                for (int i = MIN_ACCOUNT; i <= MAX_ACCOUNT; ++i) {
                    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
                }
                fflush(fPtr);
                rewind(fPtr);
            }
            int accNum = findAvailableAccount(fPtr);
            if (accNum == -1) {
                MessageBox(hDlg, "No available account numbers.", "Error", MB_OK | MB_ICONERROR);
                fclose(fPtr);
                return TRUE;
            }
            struct clientData client = {0, "", "", 0.0, "", 0};
            strncpy(client.firstName, fname, sizeof(client.firstName) - 1);
            strncpy(client.lastName, lname, sizeof(client.lastName) - 1);
            strncpy(client.pin, pin, 4); client.pin[4] = '\0';
            client.acctNum = accNum;
            client.balance = 0.0;
            client.isActive = 1;
            fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(struct clientData), SEEK_SET);
            fwrite(&client, sizeof(struct clientData), 1, fPtr);
            fflush(fPtr);
            fclose(fPtr);
            char msg[128];
            wsprintf(msg, "Account created!\nYour account number is: %d", accNum);
            MessageBox(hDlg, msg, "Success", MB_OK | MB_ICONINFORMATION);
            EndDialog(hDlg, IDOK);
            return TRUE;
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// --- Global variables ---
static int g_loggedInAccount = 0;
static struct clientData g_loggedInClient;

// Forward declarations for dialog procs and helpers
INT_PTR CALLBACK RegisterDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK LoginDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AccountDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DepositDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK WithdrawDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PinDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DeleteDlgProc(HWND, UINT, WPARAM, LPARAM);

// Helper functions
int readClient(int accNum, struct clientData* client);
int writeClient(int accNum, struct clientData* client);
void exportAccounts(HWND hwnd);
void CenterWindow(HWND hwnd);

// Main Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
            CreateWindow("BUTTON", "Register", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                40, 40, 120, 30, hwnd, (HMENU)BTN_REGISTER, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            CreateWindow("BUTTON", "Login", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                220, 40, 120, 30, hwnd, (HMENU)BTN_LOGIN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            CreateWindow("BUTTON", "Export", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                40, 100, 120, 30, hwnd, (HMENU)BTN_EXPORT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            CreateWindow("BUTTON", "Exit", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                220, 100, 120, 30, hwnd, (HMENU)BTN_EXIT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            break;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case BTN_REGISTER:
                    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_REGISTER), hwnd, RegisterDlgProc);
                    break;
                case BTN_LOGIN:
                    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_LOGIN), hwnd, LoginDlgProc);
                    break;
                case BTN_EXPORT:
                    exportAccounts(hwnd);
                    break;
                case BTN_EXIT:
                    PostQuitMessage(0);
                    break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Center a window on screen
void CenterWindow(HWND hwnd) {
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(hwnd, NULL, (sw-w)/2, (sh-h)/2, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
}

// Read client data from file
int readClient(int accNum, struct clientData* client) {
    FILE *fPtr = fopen("credit.dat", "rb");
    if (!fPtr) return 0;
    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(struct clientData), SEEK_SET);
    int ok = fread(client, sizeof(struct clientData), 1, fPtr) == 1;
    fclose(fPtr);
    return ok && client->acctNum == accNum && client->isActive;
}

// Write client data to file
int writeClient(int accNum, struct clientData* client) {
    FILE *fPtr = fopen("credit.dat", "rb+");
    if (!fPtr) return 0;
    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(struct clientData), SEEK_SET);
    int ok = fwrite(client, sizeof(struct clientData), 1, fPtr) == 1;
    fflush(fPtr);
    fclose(fPtr);
    return ok;
}

// --- Login Dialog ---
INT_PTR CALLBACK LoginDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static char accBuf[10], pinBuf[8];
    switch (message) {
    case WM_INITDIALOG:
        CenterWindow(hDlg);
        SetDlgItemText(hDlg, 401, "");
        SetDlgItemText(hDlg, 402, "");
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemText(hDlg, 401, accBuf, sizeof(accBuf));
            GetDlgItemText(hDlg, 402, pinBuf, sizeof(pinBuf));
            int accNum = atoi(accBuf);
            struct clientData client;
            
            if (accNum < MIN_ACCOUNT || accNum > MAX_ACCOUNT || strlen(pinBuf) != 4) {
                MessageBox(hDlg, "Invalid account number or PIN.", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            
            if (!readClient(accNum, &client)) {
                MessageBox(hDlg, "Account not found or deleted.", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            
            if (strcmp(client.pin, pinBuf) != 0) {
                MessageBox(hDlg, "Incorrect PIN.", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            
            g_loggedInAccount = accNum;
            g_loggedInClient = client;
            EndDialog(hDlg, IDOK);
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_ACCOUNT), GetParent(hDlg), AccountDlgProc);
            return TRUE;
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// --- Account Management Dialog ---
INT_PTR CALLBACK AccountDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG: {
        CenterWindow(hDlg);
        char buf[64];
        wsprintf(buf, "%d", g_loggedInClient.acctNum);
        SetDlgItemText(hDlg, 501, buf);
        wsprintf(buf, "%s %s", g_loggedInClient.firstName, g_loggedInClient.lastName);
        SetDlgItemText(hDlg, 502, buf);
        // Format balance with 2 decimal places
        char balanceStr[32];
        snprintf(balanceStr, sizeof(balanceStr), "%.2f", g_loggedInClient.balance);
        SetDlgItemTextA(hDlg, TEXT_BALANCE, balanceStr);
        return TRUE;
    }
    case WM_COMMAND:
        switch(LOWORD(wParam)) {
            case 601: // Deposit
                DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_DEPOSIT), hDlg, DepositDlgProc);
                break;
            case 602: // Withdraw
                DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_WITHDRAW), hDlg, WithdrawDlgProc);
                break;
            case 603: // Change PIN
                DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_PIN), hDlg, PinDlgProc);
                break;
            case 604: // Delete Account
                DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(DLG_DELETE), hDlg, DeleteDlgProc);
                break;
            case 605: // Logout
                g_loggedInAccount = 0;
                EndDialog(hDlg, IDOK);
                break;
        }
        break;
    case WM_CLOSE:
        g_loggedInAccount = 0;
        EndDialog(hDlg, IDCANCEL);
        break;
    }
    return FALSE;
}

// --- Deposit Dialog ---
INT_PTR CALLBACK DepositDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static char amtBuf[32];
    switch (message) {
    case WM_INITDIALOG:
        CenterWindow(hDlg);
        SetDlgItemText(hDlg, 701, "");
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemText(hDlg, 701, amtBuf, sizeof(amtBuf));
            double amt = atof(amtBuf);
            if (amt <= 0) {
                MessageBox(hDlg, "Invalid amount.", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            g_loggedInClient.balance += amt;
            writeClient(g_loggedInAccount, &g_loggedInClient);
            MessageBox(hDlg, "Deposit successful.", "Success", MB_OK | MB_ICONINFORMATION);
            EndDialog(hDlg, IDOK);
            return TRUE;
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// --- Withdraw Dialog ---
INT_PTR CALLBACK WithdrawDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static char amtBuf[32];
    switch (message) {
    case WM_INITDIALOG:
        CenterWindow(hDlg);
        SetDlgItemText(hDlg, 801, "");
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemText(hDlg, 801, amtBuf, sizeof(amtBuf));
            double amt = atof(amtBuf);
            if (amt <= 0 || amt > g_loggedInClient.balance) {
                MessageBox(hDlg, "Invalid or insufficient funds.", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            g_loggedInClient.balance -= amt;
            writeClient(g_loggedInAccount, &g_loggedInClient);
            MessageBox(hDlg, "Withdrawal successful.", "Success", MB_OK | MB_ICONINFORMATION);
            EndDialog(hDlg, IDOK);
            return TRUE;
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// --- Change PIN Dialog ---
INT_PTR CALLBACK PinDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static char oldPin[8], newPin[8];
    switch (message) {
    case WM_INITDIALOG:
        CenterWindow(hDlg);
        SetDlgItemText(hDlg, 901, "");
        SetDlgItemText(hDlg, 902, "");
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemText(hDlg, 901, oldPin, sizeof(oldPin));
            GetDlgItemText(hDlg, 902, newPin, sizeof(newPin));
            if (strcmp(oldPin, g_loggedInClient.pin) != 0) {
                MessageBox(hDlg, "Incorrect current PIN.", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            if (strlen(newPin) != 4) {
                MessageBox(hDlg, "PIN must be 4 digits.", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            strncpy(g_loggedInClient.pin, newPin, 4);
            g_loggedInClient.pin[4] = '\0';
            writeClient(g_loggedInAccount, &g_loggedInClient);
            MessageBox(hDlg, "PIN changed successfully.", "Success", MB_OK | MB_ICONINFORMATION);
            EndDialog(hDlg, IDOK);
            return TRUE;
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// --- Delete Account Dialog ---
INT_PTR CALLBACK DeleteDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static char pinBuf[8];
    switch (message) {
    case WM_INITDIALOG:
        CenterWindow(hDlg);
        SetDlgItemText(hDlg, 1001, "");
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemText(hDlg, 1001, pinBuf, sizeof(pinBuf));
            if (strcmp(pinBuf, g_loggedInClient.pin) != 0) {
                MessageBox(hDlg, "Incorrect PIN.", "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }
            g_loggedInClient.isActive = 0;
            writeClient(g_loggedInAccount, &g_loggedInClient);
            MessageBox(hDlg, "Account deleted.", "Success", MB_OK | MB_ICONINFORMATION);
            g_loggedInAccount = 0;
            EndDialog(hDlg, IDOK);
            EndDialog(GetParent(hDlg), IDOK); // Close account management
            return TRUE;
        } else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// --- Export accounts to accounts.txt ---
void exportAccounts(HWND hwnd) {
    FILE *fPtr = fopen("credit.dat", "rb");
    FILE *out = fopen("accounts.txt", "w");
    if (!fPtr || !out) {
        MessageBox(hwnd, "Error opening file for export.", "Error", MB_OK | MB_ICONERROR);
        if (fPtr) fclose(fPtr);
        if (out) fclose(out);
        return;
    }
    
    struct clientData client;
    fprintf(out, "%-6s %-15s %-10s %10s %-7s\n", "Acct", "Last Name", "First Name", "Balance", "Status");
    
    for (int i = MIN_ACCOUNT; i <= MAX_ACCOUNT; ++i) {
        fread(&client, sizeof(client), 1, fPtr);
        if (client.acctNum != 0) {
            fprintf(out, "%-6d %-15s %-10s %10.2f %-7s\n",
                client.acctNum, client.lastName, client.firstName, client.balance,
                client.isActive ? "Active" : "Deleted");
        }
    }
    
    fclose(fPtr);
    fclose(out);
    MessageBox(hwnd, "accounts.txt generated successfully.", "Export", MB_OK | MB_ICONINFORMATION);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc = {0};
    HWND hwnd;
    MSG Msg;
    
    // Initialize common controls
    InitCommonControls();
    
    // Manually load and initialize comctl32 version 6 if available
    HMODULE hComCtl = LoadLibrary(TEXT("comctl32.dll"));
    if (hComCtl) {
        typedef BOOL (WINAPI *PFNINITCOMMONCONTROLSEX)(const LPINITCOMMONCONTROLSEX);
        PFNINITCOMMONCONTROLSEX pfnInitCtrls = (PFNINITCOMMONCONTROLSEX)GetProcAddress(hComCtl, "InitCommonControlsEx");
        if (pfnInitCtrls) {
            INITCOMMONCONTROLSEX icc;
            icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icc.dwICC = ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES;
            pfnInitCtrls(&icc);
        }
    }

    // Register the window class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Bank Account System",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0) {
        if(!IsDialogMessage(hwnd, &Msg)) {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }
    return (int)Msg.wParam;
}
