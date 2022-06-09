#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <filesystem>

using namespace std;

struct tm newtime;
__time32_t aclock;
string getTime()
{
	//return time in hour:minute
	_time32(&aclock);
	_localtime32_s(&newtime, &aclock);
	string time = to_string(newtime.tm_hour) + ":" + to_string(newtime.tm_min);
	return time;
}
string getDate() {
	//return time in day.month.year
	_time32(&aclock);
	_localtime32_s(&newtime, &aclock);
	string date = to_string(newtime.tm_mday) + "." + to_string(1+newtime.tm_mon) + "." + to_string(1900+newtime.tm_year);
	return date;
}
//print date and time
void printTime() {
	cout << getDate() << " " << getTime() << endl;
}

//product
typedef struct product {
	string nazwa;
	int ilosc;
	int cena;
	int nr_seryjny;
};



//klasa zawierajaca metody do zarzadzania magazynem produktow
class magazyn {
public:

	friend class apteka;//tylko apteka bedzie miala dostep do prywatnych metod klasy magazyn
	//constructor that gets the name of the file and the name of the magazyn
	magazyn(string nazwa_pliku, string nazwa_magazynu) {
		this->nazwa_pliku = nazwa_pliku;
		this->nazwa_magazynu = nazwa_magazynu;
		populate_from_file(nazwa_pliku);
	}
	int ilosc_produktow;
	string nazwa_magazynu;
	//print all products
	void print_all_products() {
		for (int i = 0; i < ilosc_produktow; i++) {
			if (produkty[i].ilosc == 0) {
				goto skip; //nie wyswietlamy produktow ktore nie sa w magazynie
			}
			cout << "Nazwa: " << produkty[i].nazwa << endl;
			cout << "Nr seryjny: " << produkty[i].nr_seryjny << endl;
			cout << endl;
		skip:;
		}
	}
	//get name by nr seryjny
	string get_name_by_nr_seryjny(int nr_seryjny) {
		for (int i = 0; i < ilosc_produktow; i++) {
			if (produkty[i].nr_seryjny == nr_seryjny) {
				return produkty[i].nazwa;
			}
		}
		return "";
	}
	//save to file
	void save_to_file() {
		ofstream plik;
		plik.open(nazwa_pliku);
		for (int i = 0; i < ilosc_produktow; i++) {
			plik <<endl<< produkty[i].nazwa;
			plik <<" "<< produkty[i].ilosc;
			plik << " " << produkty[i].cena;
			plik << " " << produkty[i].nr_seryjny;
		}
		plik.close();
	}
	
private:
	friend class magazyny;
	vector<product> produkty;
	string nazwa_pliku;
	//search by nr seryjny
	void search_by_nr_seryjny(int nr_seryjny) {
		for (int i = 0; i < ilosc_produktow; i++) {
			if (produkty[i].nr_seryjny == nr_seryjny) {
				cout << "Nazwa: " << produkty[i].nazwa << endl;
				cout << "Ilosc: " << produkty[i].ilosc << endl;
				cout << "Cena: " << produkty[i].cena << endl;
				cout << "Nr seryjny: " << produkty[i].nr_seryjny << endl;
				cout << endl;
			}
		}
	}
	
	void populate_from_file(string nazwa_pliku) {
		ifstream plik;
		plik.open(nazwa_pliku);
		if (plik.good()) {
			int i = 0;
			//dodajemy produkty z listy produktow do magazynu
			while (!plik.eof()) {
				string nazwa;
				int ilosc;
				int cena;
				int nr_seryjny;
				plik >> nazwa;
				plik >> ilosc;
				plik >> cena;
				plik >> nr_seryjny;
				produkty.push_back(add_product(nazwa, ilosc, cena, nr_seryjny));
				i++;
			}
			ilosc_produktow = i;
		}
		else {
			cout << "Nie udalo sie otworzyc pliku" << endl;
		}
		plik.close();
	}
	



	//add product using constructor
	product add_product(string nazwa, int ilosc, int cena, int nr_seryjny) {
		product nowy;
		nowy.nazwa = nazwa;
		nowy.ilosc = ilosc;
		nowy.cena = cena;
		nowy.nr_seryjny = nr_seryjny;
		return nowy;
	}

	




	//remove from item ilosc
	void remove_from_item(int nr_seryjny, int ilosc) {
		for (int i = 0; i < ilosc_produktow; i++) {
			if (produkty[i].nr_seryjny == nr_seryjny) {
				produkty[i].ilosc -= ilosc;
			}
		}
	}
};

class apteka : magazyn {
private:
	friend class shopping_cart;
	friend class apteki;
	int nr_transakcji = 1;
	double vat = 1.23;
	
	
public:
	int ilosc_produktow;
	string plik_transakcji;
	string nazwa_magazynu;
	//dziedziczymy konstruktor z klasy magazyn
	apteka(string nazwa_pliku, string nazwa_magazynu, string plik_transakcji) : magazyn(nazwa_pliku, nazwa_magazynu) {
		this->plik_transakcji = plik_transakcji;
		this->ilosc_produktow = produkty.size();
		this->nazwa_magazynu = nazwa_magazynu;
		//create_file_transakcji();
		
	}
	//create file plik_transakcji if it doesn't exist or open it and count lines
	void create_file_transakcji() {
		ifstream plik;
		plik.open(plik_transakcji);
		if (plik.good()) {
			int i = 0;
			while (!plik.eof()) {
				i++;
			}
			plik.close();
			this->nr_transakcji = i;
		}
		else {
			plik.close();
			ofstream plik;
			plik.open(plik_transakcji);
			plik.close();
		}
	}
	
	//add method search by nr seryjny from magazyn
	void search_by_nr_seryjny(int nr_seryjny) {
		magazyn::search_by_nr_seryjny(nr_seryjny);
	}
	//add method print all products from magazyn
	void print_all_products() {
		magazyn::print_all_products();
	}
	void save_to_file() {
		magazyn::save_to_file();
	}
	void print_all_transactions() {
		ifstream plik;
		plik.open(plik_transakcji);
		if (plik.good()) {
			while (!plik.eof()) {
				//print plik_transakcji line by line
				string linia;
				getline(plik, linia);
				cout << linia << endl;
			}
		}
		else {
			cout << "Nie udalo sie otworzyc pliku" << endl;
		}
		plik.close();
		
	}
	//print all available product names
	virtual void print_all_product_names() {
		int h = 1;
		for (int i = 0; i < this->ilosc_produktow; i++) {
			if (produkty[i].ilosc == 0) {
				goto skip;
			}
			cout << "["<<h<<"] "<<produkty[i].nazwa <<" "<< produkty[i].nr_seryjny << endl;
			h++;
		skip:;
		}
	}
	//clear transaction file and set nr_transakcji to 1
	void clear_transaction_file() {
		//remove file plik transakcji
		remove(plik_transakcji.c_str());
		nr_transakcji = 1;
	}
	
	//transaction class including buyer, time, price, product
	//to byla klasa ale zostala zamieniona na zwyczajna funkcje
	
	void transaction(string buyer, string time, string date, int ilosc, int nr_produktu) {
		//find product by id in magazyn
		for (int i = 0; i < this->ilosc_produktow; i++) {
			if (this->produkty[i].nr_seryjny == nr_produktu) {
				//check if there is enough products
				if (this->produkty[i].ilosc >= ilosc) {
				//remove from this->produkty
				this->remove_from_item(nr_produktu, ilosc);
				
				this->save_to_file(); //save to file
				
				//calculate wartosc of the transaction
				double wartosc = ilosc * this->produkty[i].cena * this->vat; //liczenie wartoœæi, w tym wypadku cena * ilosc * vat
				ofstream plik;
				plik.open(this->plik_transakcji, ios::app);//uzywamy ios:app zeby dodawac na koniec pliku
				plik << nr_transakcji << " " << buyer << " " << date << " " << time << " " << nr_produktu << " " << wartosc << " " << this->nazwa_magazynu << endl;
				plik.close();

				nr_transakcji = nr_transakcji + 1;
				this->populate_from_file(nazwa_pliku);//refresh apteka in ram to match the file
				}
				else {
					cout << "Nie ma wystarczajacej ilosci produktu, transakcja anulowana" << endl;
				}
			}
		}
		
	}

	//function that returns name of product by id
	string get_name_by_id(int nr_seryjny) {
		for (int i = 0; i < this->ilosc_produktow; i++) {
			if (this->produkty[i].nr_seryjny == nr_seryjny) {
				return this->produkty[i].nazwa;
			}
		}
		return "";
	}
	int get_price_by_id(int nr_seryjny) {
		for (int i = 0; i < this->ilosc_produktow; i++) {
			if (this->produkty[i].nr_seryjny == nr_seryjny) {
				return this->produkty[i].cena;
			}
		}
		return 0;
	}
	
	//read recepta from file as transaction
	void read_recepta(string nazwa_recepty) {
		ifstream plik;
		plik.open(nazwa_recepty);
		if (plik.good()) {
			string buyer, time, date;
			int nr_produktu, ilosc;
			while (!plik.eof()) {
				plik >> buyer;
				plik >> nr_produktu;
				plik >> ilosc;

				transaction(buyer, getTime(), getDate(), ilosc, nr_produktu);
			}
		}
		else {
			cout << "Nie udalo sie otworzyc pliku" << endl;
		}
		plik.close();
	}
	

	//zamowienie method that checks if there is enough products in given magazyn
	void zamowienie(magazyn sprawdzany_magazyn, int ilosc, int nr_seryjny) {
		for (int i = 0; i < this->ilosc_produktow; i++) {
			if (sprawdzany_magazyn.produkty[i].nr_seryjny == nr_seryjny) {
				if (sprawdzany_magazyn.produkty[i].ilosc >= ilosc) {
					cout << "Zamowienie zostalo przyjete w magazynie "<<sprawdzany_magazyn.nazwa_magazynu << endl;
					sprawdzany_magazyn.produkty[i].ilosc -= ilosc;
					this->produkty[i].ilosc += ilosc;
					cout << "Zamowienie zostalo zrealizowane" << endl;
					cout << "W magazynie " << sprawdzany_magazyn.nazwa_magazynu << " zostalo " << sprawdzany_magazyn.produkty[i].ilosc << " " << this->get_name_by_id(nr_seryjny) << "u (" << nr_seryjny << ")" << endl;
					sprawdzany_magazyn.save_to_file();
					cout << "W aptece zostalo " << this->produkty[i].ilosc << " " << this->get_name_by_id(nr_seryjny) << "u (" << nr_seryjny << ")" << endl;
					this->save_to_file();
					break;
				}
				else {
					cout << "Nie ma wystarczajacej ilosci produktu, zamowienie anulowane" << endl;
				}
			}
		}
	}
		
	
};



void shopping_cart(apteka apt) {
		int wartosc_koszyka;
		vector<int> produkty;
		wartosc_koszyka = 0;
		produkty.clear();
		string buyer;
		cout << "Podaj imie klienta: ";
		cin >> buyer;
		system("cls");
		cout << "Witaj " << buyer << "!" << endl;
		cout << "Wybierz produkty do zakupu" << endl;
		cout << "Wpisz numer seryjny produktu zeby dodac go do koszyka" << endl;
		cout << "Wpisz 0 aby zakonczyc zakupy" << endl;
		int nr_seryjny;
		apt.print_all_product_names();
		while (true) {
			cin >> nr_seryjny;
			if (nr_seryjny == 0) {
				break;
			}
			else {
				produkty.push_back(nr_seryjny);
				cout << "Produkt zostal dodany do koszyka." << endl;
			}
		}
		cout << "Wybrano produkty: " << endl;
		for (int i = 0; i < produkty.size(); i++) {
			cout << apt.get_name_by_id(produkty[i]) << " (" << produkty[i] << ")" << endl;
			//add to wartosc koszyka
			wartosc_koszyka += apt.get_price_by_id(produkty[i]);
		}
		cout << "Wartosc koszyka (bez vat): " << wartosc_koszyka << endl;
		cout << "Czy chcesz zakupic produkty? (t/n)" << endl;
		char wybor;
		cin >> wybor;
		if (wybor == 't') {
			//create transaction for each product
			for (int i = 0; i < produkty.size(); i++) {
				apt.transaction(buyer, getTime(), getDate(), 1, produkty[i]);
			}
			//ask user if he wants to save transaction to file
			cout << "Czy chcesz zapisac paragon do pliku? (t/n)" << endl;
			char wybor;
			cin >> wybor;
			if (wybor == 't') {
				//create file

				ofstream paragon;
				string nazwa_pliku = "paragon_" + buyer + "_" + ".txt";
					paragon.open(nazwa_pliku);
					
					paragon << "Kupujacy: " << buyer << endl;
					paragon << "Data: " << getDate() << endl;
					paragon << "Godzina: " << getTime() << endl;
					paragon << "Produkty: " << endl;
					for (int i = 0; i < produkty.size(); i++) {
						paragon << apt.get_name_by_id(produkty[i]) << " (" << produkty[i] << ")" << endl;
					}
					paragon << "Wartosc zamowienia (bez vat): " << wartosc_koszyka << endl;
					paragon << "Calkowita wartosc zamowienia: " << (float)wartosc_koszyka*1.23 << endl;
					paragon << "Dziekujemy ze wybrales e-apteka!"<< endl;
					paragon.close();
					cout<<"Plik z paragonem zostal zapisany w katalogu aplikacji."<<endl;
					
				
			}
			
		}
		else {
			cout << "Zakup anulowany" << endl;
		}
	}

//klasa z wektorem produktow
//uzywam template zeby nie musiec tworzyc wielu kopii tej samej klasy dla magazynu i apteki
//technicznie moglbym tego nawet uzywac do zrobienia wektora z shopping_cart 
//ale jako ze zakladam ze kazdy uzytkownik ma jeden wozek nie bedzie to potrzebne
template<typename T> class store_vector {
	public:
		//wektor przetrzymujace dane typu T
		vector<T> magazyny_vector;
		void add_magazyn(T nowy_magazyn){
			this->magazyny_vector.push_back(nowy_magazyn);
		}
		void list_all_magazyny(){
			for (int i = 0; i < this->magazyny_vector.size(); i++) {
				cout << "[" << i + 1 << "] " << "Magazyn " << this->magazyny_vector[i].nazwa_magazynu << endl;
			}
		}
		T get_magazyn_by_number(int nr){
			return this->magazyny_vector[nr - 1];
		}

};


	
int main() {
	//tworzymy magazyn w Warszawie i apteke na ul.kaliskiego
	//wypelniamy magazyn i apteke
	magazyn magazyn_Warszawa("magazyn_Warszawa.txt", "Warszawa");
	//from store_vector class create magazyny object of type magazyn
	store_vector<magazyn> magazyny;
	//add magazyn_Warszawa to magazyny
	magazyny.add_magazyn(magazyn_Warszawa);

	apteka apteka_Kaliskiego("apteka_Kaliskiego.txt", "Kaliskiego", "transakcje_Kaliskiego.txt");
	apteka apteka_Radiowa("apteka_Radiowa.txt", "Radiowa", "transakcje_Radiowa.txt");
	store_vector<apteka> apteki;
	apteki.add_magazyn(apteka_Kaliskiego);
	apteki.add_magazyn(apteka_Radiowa);
	int wybor = 1000;
	ifstream file;
	int adminmode = 0;
	while (wybor != 0) {
	reset:
		system("cls");
		printTime();
		cout << "Witaj w aplikacji e-apteka!" << endl;
		cout << "Wybierz opcje:" << endl;
		cout << "1. Wyswietl dostepne produkty" << endl;
		cout << "2. Dodaj produkty do koszyka i wykonaj zakup" << endl;
		cout << "3. Uzyj recepty" << endl;
		cout << "5. Tryb administratora" << endl;
		cout << "0. Wyjdz z programu" << endl;
		cin >> wybor;
		
		switch (wybor) {
			case 1:
				apteki.list_all_magazyny();
				int wybor_magazynu;
				cin >> wybor_magazynu;
				system("cls");
				apteki.get_magazyn_by_number(wybor_magazynu).print_all_products();
				system("pause");
				goto reset;
			case 2:
				//"shopping cart"
				apteki.list_all_magazyny();
				int wybor_magazynu2;
				cin >> wybor_magazynu2; {
				system("cls");
				shopping_cart(apteki.get_magazyn_by_number(wybor_magazynu2));
				system("pause");
				goto reset;

			case 3:
				//wykonaj recepte dla apteki
				system("cls");
				cout << "Wpisz nazwe pliku z recepta:" << endl;
				string nazwa_pliku;
				cin >> nazwa_pliku;

				apteki.list_all_magazyny();
				int wybor_magazynu6;
				cin >> wybor_magazynu6;
				system("cls");
				apteki.get_magazyn_by_number(wybor_magazynu6).read_recepta(nazwa_pliku);
				system("pause");
				goto reset;

				}
			case 5:
				adminmode = 0;
				goto resetadmin;

		}

		if (adminmode == 1) {
			while (wybor != 0) {
			resetadmin:
			system("cls");
			printTime();
			cout << "1. Wyswietl historie transakcji" << endl;
			cout << "2. Zloz zamowienie na produkty z magazynu" << endl;
			cout << "3. Wyczysc historie transakcji apteki" << endl;
			cout << "4. Dodaj magazyn" << endl;
			cout << "5. Dodaj apteke" << endl;
			cout << "6. Wroc do trybu uzytkownika" << endl;

				cin >> wybor;

				switch (wybor) {
				case 1:
					apteki.list_all_magazyny();
					int wybor_magazynu3;
					cin >> wybor_magazynu3;
					apteki.get_magazyn_by_number(wybor_magazynu3).print_all_transactions();
					system("pause");
					goto resetadmin;
				case 2:
					//zloz zamowienie na produkt
					apteki.list_all_magazyny();
					int wybor_magazynu4;
					cin >> wybor_magazynu4;
					cout << "Prosze wybrac z ktorego magazynu zamowic produkty:" << endl;
					magazyny.list_all_magazyny();
					int wybor_magazynu41;
					cin >> wybor_magazynu41;
					system("cls");
					cout << "Wszystkie dostepne produkty w magazynie " << apteki.get_magazyn_by_number(wybor_magazynu4).nazwa_magazynu << endl;
					magazyny.get_magazyn_by_number(wybor_magazynu41).print_all_products();
					cout << "Prosze wybrac produkt do zamowienia" << endl;
					int wybor_magazynu42;
					cin >> wybor_magazynu42;
					cout << "Prosze wpisac ilosc " << magazyny.get_magazyn_by_number(wybor_magazynu41).get_name_by_nr_seryjny(wybor_magazynu42) << " do zamowienia." << endl;
					int wybor_magazynu43;
					cin >> wybor_magazynu43;
					apteki.get_magazyn_by_number(wybor_magazynu4).zamowienie(magazyny.get_magazyn_by_number(wybor_magazynu41), wybor_magazynu43, wybor_magazynu42);

					cout << "Zamowienie na " << wybor_magazynu43 << " x " << magazyny.get_magazyn_by_number(wybor_magazynu41).get_name_by_nr_seryjny(wybor_magazynu42) << " zostalo zlozone do apteki " << apteki.get_magazyn_by_number(wybor_magazynu4).nazwa_magazynu << endl;
					system("pause");
					goto resetadmin;

				case 3:
					//wybierz apteke
					apteki.list_all_magazyny();
					int wybor_magazynu5;
					cin >> wybor_magazynu5;

					//wyczysc historie transakcji
					system("cls");
					cout << "Czy na pewno chcesz wyczyscic historie transakcji? (t/n)" << endl;
					char wybor;
					cin >> wybor;
					if (wybor == 't') {
						apteki.get_magazyn_by_number(wybor_magazynu5).clear_transaction_file();
					}
					goto resetadmin;
				case 4: {
					//dodaj magazyn
					system("cls");
					cout << "Prosze wpisac nazwe magazynu:" << endl;
					string nazwa_magazynu;
					cin >> nazwa_magazynu;
					cout << "Prosze podac plik z inwentarzem magazynu:" << endl;
					string nazwa_pliku;
					cin >> nazwa_pliku;
					magazyn magazynuzytkownika = magazyn(nazwa_magazynu, nazwa_pliku);
					magazyny.add_magazyn(magazynuzytkownika);
					cout << "Magazyn " << magazynuzytkownika.nazwa_magazynu << " zostal utworzony pomyslnie." << endl;
					goto resetadmin;
				}
				case 5: {
					//dodaj apteke
					system("cls");
					cout << "Prosze wpisac nazwe apteki:" << endl;
					string nazwa_apteki;
					cin >> nazwa_apteki;
					cout << "Prosze podac plik z inwentarzem apteki:" << endl;
					string nazwa_pliku2;
					cin >> nazwa_pliku2;
					cout << "Prosze podac nazwe pliku gdzie maja byc zapisywane transakcje:" << endl;
					string nazwa_pliku3;
					cin >> nazwa_pliku3;
					apteka aptekuzytkownika = apteka(nazwa_pliku2, nazwa_apteki, nazwa_pliku3);
					apteki.add_magazyn(aptekuzytkownika);
					cout << "Apteka " << aptekuzytkownika.nazwa_magazynu << " zostala utworzona pomyslnie." << endl;
					goto resetadmin;
				}
				case 6:
					//wroc do trybu uzytkownika
					system("cls");
					goto reset;
				}
			}
		}




		return 0;

	}
}