#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>

typedef struct {
	char accountNumber[64];
	char pin[64];
	int balance;
} Account;

int loadAccountData(const char* accountNumber, Account* acc) {
	char filename[64];
	strcpy(filename, accountNumber);
	strcat(filename, ".acc");

	FILE* file = fopen(filename, "r");
	if (!file) {
		return 0; // Failed to open file
	}

	strncpy(acc->accountNumber, accountNumber, 64);

	// Read PIN on the first line of the .acc file
	if (fgets(acc->pin, sizeof(acc->pin), file) == NULL) {
		fclose(file);
		return 0;
	}
	acc->pin[strcspn(acc->pin, "\n")] = '\0';

	// Read balance on the second line
	char balanceLine[64];
	if (fgets(balanceLine, sizeof(balanceLine), file) == NULL) {
		fclose(file);
		return 0;
	}
	acc->balance = atoi(balanceLine);

	fclose(file);
	return 1;
}

void saveAccountData(const Account* acc) {
	char filename[64];
	strcpy(filename, acc->accountNumber);
	strcat(filename, ".acc");

	FILE* file = fopen(filename, "w");
	if (!file) {
		return; // Error opening file
	}

	fprintf(file, "%s\n", acc->pin);
	fprintf(file, "%d\n", acc->balance);

	fclose(file);
}

int auth(Account* acc) {
	char enteredAccount[64];
	char enteredPIN[64];

	printf("Syötä tilinumero: ");
	scanf("%s", enteredAccount);
	printf("Syötä PIN-koodi: ");
	scanf("%s", enteredPIN);

	printf("\n");
	
	// Tries to load data based on given account number and gives an error if number is wrong
	if (!loadAccountData(enteredAccount, acc)) {
		printf("Virheellinen tilinumero...\n");
		return 0;
	}
	
	// If account exists, check if PIN is correct
	if (strcmp(enteredPIN, acc->pin) == 0) {
		printf("Tunnistautuminen onnistui!\n");
		return 1;
	}

	printf("Virheellinen PIN-koodi...\n");
	printf("Poistutaan...");

	return 0;
}

void withdrawMoney(Account* acc) {
	int amount;
	printf("===== NOSTO =====\n");
	printf("Tilisi saldo on %d€\n", acc->balance);
	printf("Voit nostaa rahaa 20€, 40€, 50€ ja ylöspäin 10€ askelein. Suurin mahdollinen kertanosto on 1000€.\n");
	printf("Syötä nostettava määrä kokonaislukuna: ");
	scanf("%d", &amount);

	int new_balance = acc->balance - amount;

	// Calculate how many 50€ and 20€ bills to withdraw
	int bills_50 = amount / 50 - (amount > 50 && (int)amount % 50 % 20);
	int bills_20 = (amount - bills_50 * 50) / 20;

	if (amount <= 0 || amount > 1000 || amount != bills_50 * 50 + bills_20 * 20) {
		printf("Et voi nostaa tätä rahamäärää.\n");
		return;
	}
	else if (new_balance < 0) {
		printf("Tilillä ei ole tarpeeksi rahaa nostoon.\n");
		return;
	}

	acc->balance = new_balance;
	printf("\nNostettiin onnistuneesti %d€\n(", amount);
	if (bills_50 > 0 && bills_20 > 0) printf("%dx50€ seteliä ja %dx20€ seteliä", bills_50, bills_20);
	else if (bills_20 == 0) printf("%dx50€ seteliä", bills_50);
	else printf("%dx20€ seteliä", bills_20);
	printf(")\nUusi saldo: %d€\n", acc->balance);

	saveAccountData(acc);

	return;
}

void checkBalance(const Account* acc) {
	printf("===== TILIN SALDO =====\n");
	printf("Tilisi saldo on: %d€\n", acc->balance);

	return;
}

void showMenu(Account* acc) {
	int quit = 0;
	int choice = 0;
	do {
		printf("\n");

		printf("===== VALIKKO =====\n");
		printf("Syötä numero valitaksesi toiminnon (1-3)\n");
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
			printf("Toimintoa ei ole olemassa, yritä uudestaan.\n");
		}
	} while (choice != 3);
}

int main() {
	Account acc;

	printf("Tervetuloa käyttämään pankkiautomaattia!\n");

	if (auth(&acc)) {
		showMenu(&acc);
	}

	return 0;
}
