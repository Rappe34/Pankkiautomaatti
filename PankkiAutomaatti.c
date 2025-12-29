#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <stdbool.h>
#include <errno.h>

typedef struct {
	char accountNumber[64];
	char pin[64];
	int balance;
} AccountData;

void runATM(void);
bool getAuth(char* enteredAccount, char* enteredPIN, AccountData* acc);
bool loadAccountData(char* accountNumber, AccountData* acc);
void saveAccountData(AccountData* acc);
void withdrawMoney(AccountData* acc);
void checkBalance(AccountData* acc);
void readInt(int* n);
void readStr(char* str, size_t size);
void clearBuffer(void);

/*
main - tulostaa ohjelman käynnistysviestin ja
kutsuu ohjelman pääfunktion runATM ja
runATM-funktion palautuessa
tulostaa poistumisviestin.

Paluuarvo (int):
- 0 ohjelman palautuessa onnistuneesti (standardi main-funktion paluuarvo)
*/
int main(void) {
    printf("Tervetuloa käyttämään pankkiautomaattia!\n");

    runATM();

    printf("Poistutaan...\n");

    return 0;
}

/*
runATM - kysyy käyttäjältä tunnistautumistietoja,
kutsuu tunnistautumisfunktion getAuth.
Jos tunnistautuminen onnistuu, tulostaa päävalikon
ja kysyy käyttäjältä toimintoa.
Saatuaan syötteen, avaa käyttäjän syöttämän toiminnon.
Toistaa tätä, kunnes käyttäjä syöttää lopetusvaihtoehdon.
*/
void runATM(void) {
    bool auth;
    AccountData acc;

    char enteredAccount[64];
    char enteredPIN[64];

    printf("Syötä tilinumero: ");
    readStr(enteredAccount, 64);
    printf("Syötä PIN-koodi: ");
    readStr(enteredPIN, 64);
    printf("\n");

    auth = getAuth(enteredAccount, enteredPIN, &acc);

    if (!auth) return;

    int choice;

    do {
        printf("\n===== VALIKKO =====\n");
        printf("1. Nosta rahaa\n");
        printf("2. Tarkista saldo\n");
        printf("3. Lopeta\n");

        printf("Valitse toiminto: ");
        readInt(&choice);

        printf("\n");

        switch (choice) {
        case 1:
            withdrawMoney(&acc);
            printf("Paina mitä tahansa näppäintä jatkaaksesi...\n");
            getch();
            break;
        case 2:
            checkBalance(&acc);
            printf("Paina mitä tahansa näppäintä jatkaaksesi...\n");
            getch();
            break;
        case 3:
            break;
        default:
            printf("Virheellinen valinta.\n");
        }
    } while (choice != 3);

    saveAccountData(&acc);
}

/*
getAuth - koittaa ladata käyttäjän syöttämän tilin tiedot
kutsumalla loadAccountData-funktion.
Jos tili löytyy, vertaa käyttäjän syöttämää PIN-koodia
oikeaan PIN-koodiin ja palauttaa tunnistautumisen onnistumisen boolina.

Parametrit:
- enteredAccount (char*): pointteri käyttäjän syöttämään tilinumeroon
- enteredPIN (char*): pointteri käyttäjän syöttämään PIN-koodiin
- acc (AccountData*): pointteri muuttujaan,
johon tunnistautumistilin tiedot tallennetaan

Paluuarvo (bool):
- true, jos tunnistautuminen onnistuu
- false, jos tunnistautuminen epäonnistuu,
eli tilinumero tai PIN-koodi on virheellinen
*/
bool getAuth(char* enteredAccount, char* enteredPIN, AccountData* acc) {
    if (!loadAccountData(enteredAccount, acc)) {
        printf("Virheellinen tilinumero...\n");
        return false;
    }

    if (strcmp(enteredPIN, acc->pin) == 0) {
        printf("Oikea PIN syötetty, tunnistautuminen valmis!\n");
        return true;
    }

    printf("Virheellinen PIN-koodi...\n");
    return false;
}

/*
loadAccountData - Koittaa avata tilinumeron nimisen acc-tiedoston
ja lukea sieltä tilin PIN-koodin ja saldon.
Jos tämä onnistuu, funktio palauttaa tiedot AccountData-structin muodossa.
Jos tiedostoa ei ole olemassa, palautetaan tyhjä AccountData structi.

Parametrit:
- accountNumber (char): ladattavan tilin numero

Paluuarvo (bool):
- true, jos tilitiedosto löytyy
- false, jos tilitiedostoa ei löydy
*/
bool loadAccountData(char* accountNumber, AccountData* acc) {
	char filename[64];

	strcpy(filename, accountNumber);
	strcat(filename, ".acc");

	FILE* file = fopen(filename, "r");
	if (!file) {
		return false;
	}

	strcpy(acc->accountNumber, accountNumber);
	fgets(acc->pin, sizeof(acc->pin), file);
    acc->pin[strcspn(acc->pin, "\n")] = '\0';

    char balanceLine[64];
	fgets(balanceLine, sizeof(balanceLine), file);
    acc->balance = atoi(balanceLine);

    fclose(file);
	return true;
}

/*
saveAccountData - tallentaa tilin tiedot tiedostoon
Avaa tilinumeron nimisen acc-tiedoston
ja tallentaa sinne tilin uudet tiedot.

Parametrit:
- acc (AccountData*): pointteri tallennettavaan tiliin
*/
void saveAccountData(AccountData* acc) {
	char filename[64];
	strcpy(filename, acc->accountNumber);
	strcat(filename, ".acc");

	FILE* file = fopen(filename, "w");
	if (!file) {
		return;
	}

	fprintf(file, "%s\n", acc->pin);
	fprintf(file, "%d\n", acc->balance);

	fclose(file);
}

/*
withdrawMoney - suorittaa rahannoston tililtä
Kysyy käyttäjältä nostettavan summan, tarkistaa
noston ehdot ja päivittää tilin saldon, jos nosto
on sallittu.

Parametrit:
- acc (AccountData*): pointteri tiliin, jolta rahaa nostetaan
*/
void withdrawMoney(AccountData* acc) {
    int amount;
    printf("===== NOSTO =====\n");
    printf("Tilisi saldo on %d€\n", acc->balance);
    printf("Voit nostaa rahaa 20€, 40€, 50€ ja ylöspäin 10€ askelein. Suurin mahdollinen kertanosto on 1000€.\n");
    printf("Syötä nostettava määrä kokonaislukuna: ");
    readInt(&amount);

    printf("\n");

    int new_balance = acc->balance - amount;

    int bills_50 = amount / 50 - (amount > 50 && amount % 50 % 20);
    int bills_20 = (amount - bills_50 * 50) / 20;

    if (amount <= 0 || amount > 1000 || amount != bills_50 * 50 + bills_20 * 20) {
        printf("Et voi nostaa tätä rahamäärää.\n");
        return;
    }
    
    if (new_balance < 0) {
        printf("Tilillä ei ole tarpeeksi rahaa.\n");
        return;
    }

    acc->balance = new_balance;

    printf("Nostettiin onnistuneesti %d€\n(", amount);
	if (bills_50 > 0 && bills_20 > 0) printf("%dx50€ seteliä ja %dx20€ seteliä", bills_50, bills_20);
	else if (bills_20 == 0) printf("%dx50€ seteliä", bills_50);
	else printf("%dx20€ seteliä", bills_20);
    printf(")\nUusi saldo: %d€\n", acc->balance);
}

/*
checkBalance - tulostaa käyttäjän tilillä olevan rahamäärän.

Parametrit:
- acc (AccountData*): pointteri tarkasteltavaan tiliin
*/
void checkBalance(AccountData* acc) {
    printf("===== TILIN SALDO =====\n");
    printf("Tilisi saldo on: %d€\n", acc->balance);
}

/*
readInt - lukee käyttäjän syötteen merkkijonona readStr-funktiolla,
varmistaa että se on kokonaisluvun muotoinen,
muuttaa sen kokonaisluvuksi ja tallentaa sen
syötetyn lukupointerin muuttujaan.

Parametrit:
- n (int*): pointteri muuttujaan, johon syöte tallennetaan
*/
void readInt(int* n) {
    char input[64];
    char* endptr;

    readStr(input, sizeof(input));

    errno = 0;
    long val = strtol(input, &endptr, 10);

    if (errno == ERANGE) {
        return;
    } else if (endptr == input) {
        return;
    } else if (*endptr && *endptr != '\n') {
        return;
    }

    *n = (int)val;

    return;
}

/*
readStr - lukee käyttäjän syötteen merkkijonona,
tallentaa sen annetun merkkijonopointterin muuttujaan.
Jos syöte on liian pitkä, funktio kutsuu clearBuffer-funktion,
joka tyhjentää syötebufferin.

Parametrit:
- str (char*): pointteri merkkijonoon, johon syöte tallennetaan
- size (size_t): maksimi syötteen koko fgets-funktiota varten
*/
void readStr(char* str, size_t size) {
    if (!fgets(str, size, stdin)) {
        return;
    }

    if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = '\0';
    } else {
        clearBuffer();
    }

    return;
}

/*
clearBuffer - tyhjentää input bufferin,
joten ohjelman toiminnot eivät looppaa
useita kertoja yhden liian pitkän syötteen takia.
*/
void clearBuffer(void) {
    while (fgetc(stdin) != '\n');
}
