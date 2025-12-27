#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <stdbool.h>

typedef struct {
	char accountNumber[64];
	char pin[64];
	int balance;
} AccountData;

AccountData loadAccountData(const char* accountNumber) {
	AccountData acc = { 0 };
	char filename[64];

	strcpy(filename, accountNumber);
	strcat(filename, ".acc");

    // Try to get account data from file with account number as name
	FILE* file = fopen(filename, "r");
	if (!file) {
        // Account doesn't exist, return empty acc data
		acc.accountNumber[0] = '\0';
		return acc;
	}

	strcpy(acc.accountNumber, accountNumber);

    // Read PIN on the first line of the acc file
	fgets(acc.pin, sizeof(acc.pin), file);
    acc.pin[strcspn(acc.pin, "\n")] = '\0';

    // Read balance from the second line
    char balanceLine[64];
	fgets(balanceLine, sizeof(balanceLine), file);
    acc.balance = atoi(balanceLine);

    fclose(file);
	return acc;
}

void saveAccountData(const AccountData* acc) {
	char filename[64];
	strcpy(filename, acc->accountNumber);
	strcat(filename, ".acc");

	FILE* file = fopen(filename, "w");
	if (!file) {
		return; // Error opening acc file
	}

    // Overwrite new account data to the acc file
	fprintf(file, "%s\n", acc->pin);
	fprintf(file, "%d\n", acc->balance);

	fclose(file);
}

bool getAuth(AccountData* acc) {
    char enteredAccount[64];
    char enteredPIN[64];

    printf("Syötä tilinumero: ");
    scanf("%s", enteredAccount);
    printf("Syötä PIN-koodi: ");
    scanf("%s", enteredPIN);
    printf("\n");

    *acc = loadAccountData(enteredAccount);

    // Failed to find the acc file for the given account number
    if (acc->accountNumber[0] == '\0') {
        printf("Virheellinen tilinumero...\n");
        return false;
    }

    // Check if entered PIN matches with the account PIN
    if (strcmp(enteredPIN, acc->pin) == 0) {
        printf("Tunnistautuminen onnistui!\n");
        strcpy(acc->accountNumber, enteredAccount);
        return true; // Return succesful auth
    }

    // Return failed auth
    printf("Virheellinen PIN-koodi...\n");
    return false;
}

void withdrawMoney(AccountData* acc) {
    int amount;
    printf("===== NOSTO =====\n");
    printf("Tilisi saldo on %d€\n", acc->balance);
    printf("Voit nostaa rahaa 20€, 40€, 50€ ja ylöspäin 10€ askelein. Suurin mahdollinen kertanosto on 1000€.\n");
    printf("Syötä nostettava määrä kokonaislukuna: ");
    scanf("%d", &amount);

    int new_balance = acc->balance - amount;

    // Calculate number of 50€ and 20€ bills to withdraw
    int bills_50 = amount / 50 - (amount > 50 && amount % 50 % 20);
    int bills_20 = (amount - bills_50 * 50) / 20;

    // Make sure withdrawal amount can be made up with 50€ and 20€ bills, if not throw error
    if (amount <= 0 || amount > 1000 || amount != bills_50 * 50 + bills_20 * 20) {
        printf("Et voi nostaa tätä rahamäärää.\n");
        return;
    }
    // Throw error if account doesn't have enough money
    if (new_balance < 0) {
        printf("Tilillä ei ole tarpeeksi rahaa.\n");
        return;
    }

    acc->balance = new_balance;

    printf("\n");

    printf("Nostettiin onnistuneesti %d€\n(", amount);
	if (bills_50 > 0 && bills_20 > 0) printf("%dx50€ seteliä ja %dx20€ seteliä", bills_50, bills_20);
	else if (bills_20 == 0) printf("%dx50€ seteliä", bills_50);
	else printf("%dx20€ seteliä", bills_20);
    printf(")\nUusi saldo: %d€\n", acc->balance);
}

void checkBalance(const AccountData* acc) {
    printf("===== TILIN SALDO =====\n");
    printf("Tilisi saldo on: %d€\n", acc->balance);
}

void showMenu(AccountData* acc) {
    int choice;

    do {
        printf("\n===== VALIKKO =====\n");
        printf("1. Nosta rahaa\n");
        printf("2. Tarkista saldo\n");
        printf("3. Lopeta\n");

        printf("Valitse toiminto: ");
        scanf("%d", &choice);

        printf("\n");

        switch (choice) {
        case 1:
            withdrawMoney(acc);
            printf("Paina mitä tahansa näppäintä jatkaaksesi...\n");
            getch();
            break;
        case 2:
            checkBalance(acc);
            printf("Paina mitä tahansa näppäintä jatkaaksesi...\n");
            getch();
            break;
        case 3:
            printf("Poistutaan...\n");
            break;
        default:
            printf("Virheellinen valinta.\n");
        }
    } while (choice != 3);
}

int main(void) {
    bool auth;
    AccountData acc;

    printf("Tervetuloa käyttämään pankkiautomaattia!\n");

    auth = getAuth(&acc);

    if (auth) {
        showMenu(&acc);
    }

    saveAccountData(&acc);

    return 0;
}
