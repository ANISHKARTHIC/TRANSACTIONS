#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MIN_ACCOUNT 100
#define MAX_ACCOUNT 999

// clientData structure definition
struct clientData {
    unsigned int acctNum; // account number (3 digits)
    char lastName[15];
    char firstName[10];
    double balance;
    char pin[5];          // 4-digit PIN
    int isActive;         // 1 = active, 0 = deleted
};

// Function prototypes
void registerAccount(FILE *fPtr);
int loginAccount(FILE *fPtr);
void updateTextFile(FILE *fPtr);
void showMainMenu(FILE *fPtr, int loggedInAccount);
void withdraw(FILE *fPtr, int account);
void deposit(FILE *fPtr, int account);
void checkBalance(FILE *fPtr, int account);
void deleteAccount(FILE *fPtr, int account);
int validatePin(FILE *fPtr, int accountNum, const char *enteredPin);
int changePin(FILE *fPtr, int accountNum);
int findAvailableAccount(FILE *fPtr);

int main() {
    FILE *cfPtr;
    int loggedInAccount = 0;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        cfPtr = fopen("credit.dat", "wb+");
        if (cfPtr == NULL) {
            printf("File could not be created.\n");
            exit(1);
        }

        struct clientData blankClient = {0, "", "", 0.0, "", 0};
        for (int i = MIN_ACCOUNT; i <= MAX_ACCOUNT; ++i) {
            fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
        }
        rewind(cfPtr);
    }

    int choice;
    while (1) {
        printf("\n--- Bank Account System ---\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Export to Text File\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: registerAccount(cfPtr); break;
            case 2:
                loggedInAccount = loginAccount(cfPtr);
                if (loggedInAccount > 0) {
                    showMainMenu(cfPtr, loggedInAccount);
                }
                break;
            case 3: updateTextFile(cfPtr); break;
            case 4:
                fclose(cfPtr);
                printf("Exiting. Goodbye!\n");
                exit(0);
            default: printf("Invalid choice.\n");
        }
    }
}

// Register new account
void registerAccount(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0, "", 0};
    int accNum = findAvailableAccount(fPtr);
    if (accNum == -1) {
        printf("No available account numbers.\n");
        return;
    }

    printf("\n--- Register Account ---\n");
    printf("First Name: ");
    scanf("%9s", client.firstName);
    printf("Last Name: ");
    scanf("%14s", client.lastName);

    while (1) {
        printf("Enter 4-digit PIN: ");
        scanf("%4s", client.pin);
        if (strlen(client.pin) == 4) break;
        printf("PIN must be exactly 4 digits.\n");
    }

    client.acctNum = accNum;
    client.balance = 0.0;
    client.isActive = 1;

    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    fflush(fPtr);

    printf("Account created successfully. Your account number is %d\n", accNum);
}

// Login to existing account
int loginAccount(FILE *fPtr) {
    int accNum;
    char pin[5];
    struct clientData client;

    printf("\n--- Login ---\n");
    printf("Account number: ");
    scanf("%d", &accNum);
    if (accNum < MIN_ACCOUNT || accNum > MAX_ACCOUNT) {
        printf("Invalid account number.\n");
        return 0;
    }

    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum == 0 || !client.isActive) {
        printf("Account not found or deleted.\n");
        return 0;
    }

    printf("PIN: ");
    scanf("%4s", pin);
    if (validatePin(fPtr, accNum, pin)) {
        printf("Login successful. Welcome, %s %s!\n", client.firstName, client.lastName);
        return accNum;
    } else {
        printf("Incorrect PIN.\n");
        return 0;
    }
}

void showMainMenu(FILE *fPtr, int accNum) {
    int choice;
    while (1) {
        printf("\n--- Account Menu [%d] ---\n", accNum);
        printf("1. Deposit\n");
        printf("2. Withdraw\n");
        printf("3. Check Balance\n");
        printf("4. Change PIN\n");
        printf("5. Delete Account\n");
        printf("6. Logout\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: deposit(fPtr, accNum); break;
            case 2: withdraw(fPtr, accNum); break;
            case 3: checkBalance(fPtr, accNum); break;
            case 4: changePin(fPtr, accNum); break;
            case 5: deleteAccount(fPtr, accNum); return;
            case 6: return;
            default: printf("Invalid choice.\n");
        }
    }
}

void deposit(FILE *fPtr, int accNum) {
    struct clientData client;
    double amt;

    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    printf("Enter amount to deposit: ");
    scanf("%lf", &amt);
    if (amt <= 0) {
        printf("Invalid amount.\n");
        return;
    }

    client.balance += amt;
    fseek(fPtr, -(long int)sizeof(client), SEEK_CUR);
    fwrite(&client, sizeof(client), 1, fPtr);
    fflush(fPtr);

    printf("Deposit successful. New balance: %.2f\n", client.balance);
}

void withdraw(FILE *fPtr, int accNum) {
    struct clientData client;
    double amt;

    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    printf("Enter amount to withdraw: ");
    scanf("%lf", &amt);
    if (amt <= 0 || amt > client.balance) {
        printf("Invalid or insufficient funds.\n");
        return;
    }

    client.balance -= amt;
    fseek(fPtr, -(long int)sizeof(client), SEEK_CUR);
    fwrite(&client, sizeof(client), 1, fPtr);
    fflush(fPtr);

    printf("Withdrawal successful. New balance: %.2f\n", client.balance);
}

void checkBalance(FILE *fPtr, int accNum) {
    struct clientData client;
    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);
    printf("Current Balance: %.2f\n", client.balance);
}

int changePin(FILE *fPtr, int accNum) {
    struct clientData client;
    char oldPin[5], newPin[5];

    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    printf("Enter current PIN: ");
    scanf("%4s", oldPin);
    if (strcmp(oldPin, client.pin) != 0) {
        printf("Incorrect PIN.\n");
        return 0;
    }

    printf("Enter new 4-digit PIN: ");
    scanf("%4s", newPin);
    if (strlen(newPin) != 4) {
        printf("PIN must be 4 digits.\n");
        return 0;
    }

    strcpy(client.pin, newPin);
    fseek(fPtr, -(long int)sizeof(client), SEEK_CUR);
    fwrite(&client, sizeof(client), 1, fPtr);
    fflush(fPtr);

    printf("PIN changed successfully.\n");
    return 1;
}

void deleteAccount(FILE *fPtr, int accNum) {
    struct clientData client;
    char pin[5];

    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    printf("Enter PIN to confirm deletion: ");
    scanf("%4s", pin);
    if (strcmp(pin, client.pin) != 0) {
        printf("Incorrect PIN.\n");
        return;
    }

    client.isActive = 0;
    fseek(fPtr, -(long int)sizeof(client), SEEK_CUR);
    fwrite(&client, sizeof(client), 1, fPtr);
    fflush(fPtr);

    printf("Account deleted.\n");
}

int validatePin(FILE *fPtr, int accNum, const char *enteredPin) {
    struct clientData client;
    fseek(fPtr, (accNum - MIN_ACCOUNT) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);
    return strcmp(enteredPin, client.pin) == 0;
}

void updateTextFile(FILE *fPtr) {
    FILE *out = fopen("accounts.txt", "w");
    if (!out) {
        printf("Error opening accounts.txt\n");
        return;
    }

    struct clientData client;
    rewind(fPtr);

    fprintf(out, "%-6s %-15s %-10s %10s %-6s %-7s\n", 
            "Acct", "Last Name", "First Name", "Balance", "PIN", "Status");

    for (int i = MIN_ACCOUNT; i <= MAX_ACCOUNT; ++i) {
        fread(&client, sizeof(client), 1, fPtr);
        if (client.acctNum != 0) {
            fprintf(out, "%-6d %-15s %-10s %10.2f %-6s %-7s\n",
                    client.acctNum, client.lastName, client.firstName,
                    client.balance, client.pin,
                    client.isActive ? "Active" : "Deleted");
        }
    }

    fclose(out);
    printf("accounts.txt generated successfully.\n");
}

int findAvailableAccount(FILE *fPtr) {
    struct clientData client;
    rewind(fPtr);
    for (int i = MIN_ACCOUNT; i <= MAX_ACCOUNT; ++i) {
        fread(&client, sizeof(client), 1, fPtr);
        if (client.acctNum == 0) return i;
    }
    return -1;
}
