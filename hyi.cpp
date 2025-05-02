#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
using namespace std;

//|-----------------------------CLASSES----------------------------|
const int STR_SIZE = 32;

class Person {
    char name[STR_SIZE];
    char surname[STR_SIZE];
    char lastname[STR_SIZE];
public:
    Person() { name[0] = surname[0] = lastname[0] = 0; }

    Person(const char* n, const char* s, const char* l) {
        strncpy(name, n, STR_SIZE); name[STR_SIZE - 1] = 0;
        strncpy(surname, s, STR_SIZE); surname[STR_SIZE - 1] = 0;
        strncpy(lastname, l, STR_SIZE); lastname[STR_SIZE - 1] = 0;
    }
    Person(const Person& p) {
        strncpy(name, p.name, STR_SIZE);
        strncpy(surname, p.surname, STR_SIZE);
        strncpy(lastname, p.lastname, STR_SIZE);
    }
    void inputFromConsole() {
        cout << "Enter Name: "; cin >> name;
        cout << "Enter Surname: "; cin >> surname;
        cout << "Enter Lastname: "; cin >> lastname;
    }

    friend ostream& operator<<(ostream& os, const Person& p) {
        return os << p.name << " " << p.surname << " " << p.lastname;
    }

    string getName() const { return name; }
    string getSurname() const { return surname; }
    string getLastname() const { return lastname; }
};

class Worker : public Person {
    int ID;
    char position[STR_SIZE];
public:
    Worker() : ID(0) { position[0] = 0; }
    Worker(const Person& p, const char* pos, int ID) : Person(p), ID(ID) {
        strncpy(position, pos, STR_SIZE); position[STR_SIZE - 1] = 0;
    }
    void ShowInfo() {
        cout << "\n|-------------------|\nName: " << getName() << "\nSurname: " << getSurname() << "\nLastname: " << getLastname() << "\nID: " << ID << "\nPosition: " << position << endl;
    }
    int getID() const { return ID; }
    string getPosition() const { return position; }
};
class Client : public Person {
    int balance=0;
    int clientID;
public:
    Client() = default;
    Client(const Person& p, int clientID) : Person(p), clientID(clientID){}
    void ShowInfo() {
        cout << "\n|-------------------|\nName: " << getName() << "\nSurname: " << getSurname() << "\nLastname: " << getLastname() << "\nID: " << clientID << "\nBalance: " << balance << endl;
    }
    int getClientID() const { return clientID; }
    int getClientBalance() { return balance; }
    void decreaseClientBalance(int num) { balance -= num; }
    void increaseClientBalance(int num) { balance += num; }
};
//головна система, має містити в собі головний баланс який буде визначати функціональність банку
class Bank {
    int bankID;
    int money = 0;
    char name[20];
public:
    Bank() = default;
    Bank(int money, const char name[]) : money(money) { strcpy(this->name, name); }
    void showInfo() { cout << "\n|-------------------|\nBank name: " << name << "\nBank money: " << money; }
    void setMoney(int money) { this->money = money; }
    void setName(const char name[]) { strcpy(this->name, name); }
    void setBankID(int bankID) { this->bankID = bankID; }
    int getBankID() { return bankID; }
    int getAllBankMoney(vector<BankBranch>& branches) { int sum = money;for (auto& a : branches)if (a.getBranchID() == bankID)sum+=a.getBranchMoney(); }

    int WithdrawMoney(int num) { if (money - num > 0) { money -= num; return 0; } else return 1; }
    void PutMoney(int num, int balance) { if (balance < num)cout << "You have no money!\n"; }
};
//через відділення банку можна звязувати з головним банком та запитувати доступ до операцій таких як покласти гроші зняти гроші кредит і депозит
//коротко кажучи саме через цю штуку будуть відбуватися всі операції
//на операції можуть робити запити робітники банку тим самим вони роблять реквест до відділення відділення звязується з банком і видає результат операції 
class BankBranch {
    int bankID=0;
    Bank* bank_ptr=nullptr;
    int money;
public:
    BankBranch() = default;
    BankBranch(int bankID):bankID(bankID){}

    void setBankID(int bankID) { this->bankID = bankID; }
    int getBranchID() { return bankID; }
    void setBankPointer(Bank* b) { bank_ptr = b; }
    int getBranchMoney() { return money; }

    void WithdrawMoney(int num) { int responce = bank_ptr->WithdrawMoney(num); if (responce == 0)cout << "WithdrawComplete!\n";else if (responce == 1)cout << "Bank dont have money!\n";else cout << "Unknown error!\n"; }
    void PutMoney(int num, Client& cl, vector<BankBranch>& branches) { if (cl.getClientBalance() < num)cout << "You have no money!\n";else { if (num > this->money && num <= bank_ptr->getAllBankMoney(branches)); } };
}

};

//|-----------------------------CLASSES----------------------------|



//|----------------------Запис Читання з файлу-------------------|
template <typename T>
void writeToFile(const char* filename, const vector<T>& data) {
    ofstream file(filename, ios::binary);
    for (const auto& obj : data) {
        file.write((char*)(&obj), sizeof(T));
    }
    file.close();
}

template <typename T>
void readFromFile(const char* filename, vector<T>& data) {
    ifstream file(filename, ios::binary);
    data.clear();
    T obj;
    while (file.read((char*)(&obj), sizeof(T))) {
        data.push_back(obj);
    }
    file.close();
}
//|----------------------Запис Читання з файлу-------------------|
//перевірка на унікальність айді при створенні воркера
bool isIDUnique(int id, vector<Worker>& wk) {
    for (auto& w : wk) if (w.getID() == id) return false;
    return true;
}
//сама перша менюшка
void showMenu() {
    cout << "\n==== MENU ====\n";
    cout << "1. Load people from file\n";
    cout << "2. Add new person\n";
    cout << "3. Show all people\n";
    cout << "4. Save to file\n";
    cout << "5. Exit\n";
    cout << "6. Create worker from person\n";
    cout << "7. BankTest";
    cout << "8. Client Test Menu";
    cout << "Choose: ";
}
//тестове банк меню
void BankTestMenu() {
    cout << "\n==== MENU ====\n";
    cout << "1. Enter money to bank\n";
    cout << "2. Enter name for bank\n";
    cout << "3. Save to file\n";
    cout << "4. Read from file\n";
    cout << "5. Create Bank\n";
    cout << "6. List all banks\n";
    cout << "7. Exit\n";
}
//тестове меню відділень
void BranchTestMenu() {
    cout << "\n==== MENU ====\n";
    cout << "1. Enter money to bank\n";
    cout << "2. Enter name for bank\n";
    cout << "3. Save to file\n";
    cout << "4. Read from file\n";
    cout << "5. Create Bank\n";
    cout << "6. List all banks\n";
    cout << "7. Exit\n";
}
void clientTestMenu() {
    cout << "\n==== MENU ====\n";
    cout << "1. Load client from file\n";
    cout << "2. Add new client\n";
    cout << "3. Show all client\n";
    cout << "4. Save to file\n";
    cout << "5. Exit\n";
    cout << "Choose: ";
}
//функція для знаходження актуального вказівника на головний банк по айді
void linkBank(vector<BankBranch>& branches,vector<Bank>& banks) {
    for (auto it = branches.begin(); it != branches.end(); ) {
        BankBranch& branch = *it;
        bool found = false;
        for (auto& bank : banks) {
            if (branch.getBranchID() == bank.getBankID()) {
                found = true;
                branch.setBankPointer(&bank);cout << "\nFound!\n";
            }
        }
        if (!found) {
            cout << "WARNING: Branch ID " << branch.getBranchID() << " has no matching bank!\n";
            cout << "Do you want to:\n";
            cout << "  [1] Link to existing bank manually\n";
            cout << "  [2] Delete this branch\n";
            cout << "Your choice: ";int choice;cin >> choice;
            if (choice == 1) {
                cout << "Chose for what bank you want to link branch!\n";int n=1;
                for (auto& a : banks) {
                    cout << n++ << ')';
                        a.showInfo();
                }
                cout << "Enter bank num to link: "; int num; cin >> num;
                if (num >= 1 && num <= banks.size()) {
                    branch.setBankID(banks[num - 1].getBankID());
                    branch.setBankPointer(&banks[num - 1]);
                    ++it;
                }
            }
            else if(choice==2) {
                it = branches.erase(it);
                cout << "Succesfully deleted branch\n";
            }
            else { ++it; }
        }
        else { ++it; }
    }
}
void linkBank(vector<BankBranch>& branches, vector<Bank>& banks) {
    for (auto it = branches.begin(); it != branches.end(); ) {
        BankBranch& branch = *it;
        bool found = false;

        for (auto& bank : banks) {
            if (branch.getBranchID() == bank.getBankID()) {
                branch.setBankPointer(&bank);
                found = true;
                cout << "Linked branch ID " << branch.getBranchID() << " to bank.\n";
                break;
            }
        }

        if (!found) {
            cout << "WARNING: Branch ID " << branch.getBranchID() << " has no matching bank!\n";
            cout << "Do you want to:\n";
            cout << "  [1] Link to existing bank manually\n";
            cout << "  [2] Delete this branch\n";
            cout << "Your choice: ";
            int choice;
            cin >> choice;

            if (choice == 1) {
                cout << "Available banks:\n";
                for (auto& bank : banks) {
                    cout << "  ID: " << bank.getBankID() << "\n";
                }

                cout << "Enter bank ID to link: ";
                int id;
                cin >> id;

                auto foundBank = std::find_if(banks.begin(), banks.end(), [id](const Bank& b) {
                    return b.getBankID() == id;
                    });

                if (foundBank != banks.end()) {
                    branch.setBankPointer(&(*foundBank));
                    cout << "Branch linked to bank ID " << id << ".\n";
                    ++it; // move to next element
                }
                else {
                    cout << "Invalid bank ID. Branch not linked.\n";
                    it = branches.erase(it); // delete this branch
                }

            }
            else if (choice == 2) {
                cout << "Branch ID " << branch.getBranchID() << " deleted.\n";
                it = branches.erase(it); // remove current branch
            }
            else {
                cout << "Invalid option. Branch skipped.\n";
                ++it;
            }

        }
        else {
            ++it; // move to next element
        }
    }
}



//|----------------------Functions----------------------|

int main() {
    vector<Person> people;
    vector<Worker> worker;
    vector<Client> client;
    vector<Bank> bank;
    vector<BankBranch> branch;
    const char* filename = "person.bin";
    const char* worker_filename = "worker.bin";
    const char* bank_filename = "bank.bin";
    const char* branch_filename = "branch.bin";
    const char* client_filename = "client.bin";
    int choice;

//|-------------AUTOSTART--------------|
    linkBank(branch,bank);
//|-------------AUTOSTART--------------|  

    while (true) {
        showMenu();
        cin >> choice;

        if (choice == 1) {
            people.clear();
            worker.clear();
            readFromFile(filename, people);
            readFromFile(worker_filename, worker);
            cout << "Loaded " << people.size() << " people from file.\n";
        }
        else if (choice == 2) {
            Person newPerson;
            newPerson.inputFromConsole();
            people.push_back(newPerson);
            cout << "Person added.\n";
        }
        else if (choice == 3) {
            int lol; cout << "1)Show people\n2)Show Worker\n"; cin >> lol;
            if (lol == 1)
                if (people.empty()) {
                    cout << "No people in memory.\n";
                }
                else {
                    cout << "People list:\n";
                    for (const auto& p : people) {
                        cout << p << endl;
                    }
                }
            else if (lol == 2)
                if (worker.empty()) {
                    cout << "No worker in memory.\n";
                }
                else {
                    cout << "Worker list:\n";
                    for (auto& p : worker) {
                        p.ShowInfo();
                    }
                }

        }
        else if (choice == 4) {
            writeToFile(filename, people);
            writeToFile(worker_filename, worker);
            cout << "Data saved to file.\n";
        }
        else if (choice == 5) {
            cout << "Exiting...\n";
            break;
        }
        else if (choice == 6) {
            int count = 1, chose; cout << "Chose person to do worker:\n"; char position[STR_SIZE];
            for (auto& a : people) {
                cout << count << ")" << "Name: " << a.getName() << "\nSurname: " << a.getSurname() << "\nLastname: " << a.getLastname() << endl; count++;
            } cout << count << ")Create new person\nchoice: ";
            cin >> chose;
            if (chose == count) {
                Person newPerson;
                newPerson.inputFromConsole();
                people.push_back(newPerson);
                cout << "Person added.\n";
                cout << "Enter position: ";
                cin >> position;
                int tempid;
                while (true) {
                    cout << "Enter ID: "; cin >> tempid; if (isIDUnique(tempid, worker)) break; cout << "Enter another id thats ID corrupted: ";
                }
                Worker tempWorker(newPerson, position, tempid);
                worker.push_back(tempWorker);
            }
            else {
                cout << "Enter position: ";
                cin >> position;
                int tempid;
                while (true) {
                    cout << "Enter ID: "; cin >> tempid; if (isIDUnique(tempid, worker)) break; cout << "Enter another id thats ID corrupted: ";
                }
                Worker tempWorker(people[chose - 1], position, tempid);
                worker.push_back(tempWorker);
            }
        }
        else if (choice == 7) {
            while (true) {
                BankTestMenu();int bankchoice;
                cout << "Enter your choice: ";cin >> bankchoice;
                if (bankchoice == 1) {
                    int lol = 1, prikol, money;
                    for (auto& a : bank) {
                        cout << lol << ')';
                        a.showInfo();
                        cin >> prikol;lol++;
                        cout << "Enter ammount of money: ";cin >> money;
                        bank[prikol].setMoney(money);
                    }
                }
                else if (bankchoice == 2) {
                    int lol = 1, prikol;char name[20];
                    for (auto& a : bank) {
                        cout << lol << ')';
                        a.showInfo();
                        cin >> prikol;lol++;
                        cout << "Enter name: ";cin >> name;
                        bank[prikol].setName(name);
                    }
                }
                else if (bankchoice == 3) {
                    writeToFile(bank_filename, bank);
                }
                else if (bankchoice == 4) {
                    readFromFile(bank_filename, bank);
                    linkBank(branch, bank);
                }
                else if (bankchoice == 5) {
                    int money;char name[20];cout << "Enter amout of money: ";cin >> money;cout << "Enter name: ";cin >> name;cin.ignore();
                    Bank newBank(money, name);
                    bank.push_back(newBank);
                }
                else if (bankchoice == 6) {
                    if (bank.empty())cout << "There is no banks...\n";
                    else for (auto& a : bank) {
                        a.showInfo();
                    }
                }
                else if (bankchoice == 7)break;
                else {
                    cout << "Invalid option!\n";
                }
            }
        }
        else {
            cout << "Invalid option!\n";
        }
    }
    return 0;
}