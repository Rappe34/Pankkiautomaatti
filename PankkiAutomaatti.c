#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>

int userAuth();
int mainMenu();
int withdrawMoney();
void checkBalance();
int getAccountBalance();
int deductFromBalance();

int main() {
	int mainLoop = 1;
	int auth = 0;
	char authAcc[64];

	printf("Tervetuloa käyttämään pankkiautomaattia!\n");

	while (mainLoop) {
		auth = (userAuth(authAcc) == 0);

		if (!auth) {
			break;
		}

		while (auth) {
			if (mainMenu(authAcc) != 0) {
				auth = 0;
				mainLoop = 0;
			}
		}
	}

	return 0;
}

int userAuth(char accNum[]) {
	char inputPin[64];
	char realPin[64];
	FILE* file;

	printf("Syötä tilinumero: ");
	scanf("%s", accNum);

	strcat(accNum, ".acc");
	file = fopen(accNum, "r");

	if (file != NULL) {
		printf("Syötä PIN-koodi: ");
		scanf("%s", &inputPin);

		fgets(realPin, 64, file);
		fclose(file);

		if (realPin[strlen(realPin) - 1] == '\r' || realPin[strlen(realPin) - 1] == '\n') {
			realPin[strlen(realPin) - 1] = '\0';
		}

		if (strcmp(inputPin, realPin) == 0) {
			printf("\nOikea PIN syötetty, tunnistautuminen valmis!\n");
			return 0;
		}
		else {
			printf("Virheellinen PIN...\n");
		}
	}
	else {
		printf("Virheellinen tilinumero...\n");
	}

	return 1;
}

int mainMenu(char authAcc[]) {
	int returnCall = 0;
	int actionChoice;

	printf("\n===== VALIKKO =====\n");
	printf("1. Nosta rahaa\n");
	printf("2. Tarkista saldo\n");
	printf("3. Lopeta\n");

	printf("Valitse toiminto: ");
	scanf("%d", &actionChoice);

	printf("\n");

	switch (actionChoice)
	{
	case 1:
		if (withdrawMoney(authAcc) == 0) {
			returnCall = 1;
		}
		break;
	case 2:
		checkBalance(authAcc);
		break;
	case 3:
		printf("Poistutaan...\n");
		return 1;
	default:
		printf("Toimintoa ei ole olemassa.\n");
		break;
	}

	printf("\nPaina mitä tahansa näppäintä jatkaaksesi.\n");
	_getch();

	if (returnCall) {
		printf("\nPoistutaan...\n");
		return 1;
	}

	printf("\nPalataan päävalikkoon...\n");

	return 0;
}

int withdrawMoney(char accFile[]) {
	int balance = getAccountBalance(accFile);
	int wdAmount;

	printf("===== NOSTO =====\n");
	printf("Voit nostaa rahaa 20€, 40€, 50€ ja ylöspäin 10€ askelein. Suurin mahdollinen kertanosto on 1000€.\n");
	printf("Tilisi saldo on %d€\n", balance);
	printf("Syötä nostettava määrä kokonaislukuna: ");
	scanf("%d", &wdAmount);

	int bills_50 = wdAmount / 50 - (wdAmount > 50 && wdAmount % 50 % 20);
	int bills_20 = (wdAmount - bills_50 * 50) / 20;
	if (wdAmount <= 0 || wdAmount > 1000 ||wdAmount != bills_50 * 50 + bills_20 * 20) {
		printf("Et voi nostaa tätä rahamäärää.\n");
	}
	else if (balance < 0) {
		printf("Tilillä ei ole tarpeeksi rahaa nostoon.\n");
	}
	else {
		if (deductFromBalance(accFile, wdAmount) == 0) {
			printf("\nNostettiin onnistuneesti %d€\n(", wdAmount);
			if (bills_50 > 0 && bills_20 > 0) printf("%dx50€ seteliä ja %dx20€ seteliä", bills_50, bills_20);
			else if (bills_20 == 0) printf("%dx50€ seteliä", bills_50);
			else printf("%dx20€ seteliä", bills_20);
			printf(")\nUusi saldo: %d€\n", getAccountBalance(accFile));

			return 0;
		}
	}

	return 1;
}

void checkBalance(char accFile[]) {
	printf("===== TILIN SALDO =====\n");
	printf("Tilin saldo on: %d€\n", getAccountBalance(accFile));
}

int getAccountBalance(char accFile[]) {
	FILE* file = fopen(accFile, "r");

	char balanceString[64];

	fgets(balanceString, 64, file);
	fgets(balanceString, 64, file);

	int balance = atoi(balanceString);

	return balance;
}

int deductFromBalance(char accFile[], int amount) {
	int balance = getAccountBalance(accFile);
	balance -= amount;

	FILE* file = fopen(accFile, "r");

	char pin[64];
	fgets(pin, 64, file);
	if (pin[strlen(pin) - 1] == '\n') {
		pin[strlen(pin) - 1] = '\0';
	}
	fclose(file);

	file = fopen(accFile, "w");
	fprintf(file, "%s\n%d", pin, balance);
	fclose(file);

	return 0;
}
