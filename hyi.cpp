#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <ctime>
#include <string>
#include <math.h>
#include <Windows.h>

using namespace std;

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} dtime;

class Account {
    char login[20]{};
    char password[20]{};
    char role[20]{};
    int PID{};
    char TelegramID[20]{};
    bool verified=false;
public:
    Account() = default;
    Account(const char* login, const char* password, const char* role,int PID):PID(PID) { strcpy(this->login, login);strcpy(this->password, password);strcpy(this->role, role); }
    const char* getLogin() { return login; }
    const char* getPassword() { return password; }
    string getRole() { return role; }
    int getPersonID() { return PID; }
    friend ostream& operator<<(ostream& os, const Account& p) {
        return os << p.login << " " << p.password << " " << p.role;
    }
    void setVerified(bool b) { if (b)verified = true; }
    void setTelegramID(string num) { strcpy(TelegramID, num.c_str()); }
    string getTelegramID() { return TelegramID; }
    void setPassword(const char* pass) {strcpy(password, pass);}
};
Account* accPtr{};

//|-----------------------------CLASSES----------------------------|
const int STR_SIZE = 32;
class BankBranch;
class Client;
class Bank;
int AdminBranchesMenu(vector<BankBranch>& branches, Bank& bank);
char curUser[20];
dtime tmToCtime(const tm& timeinfo);
tm ctimeToTm(const dtime& ct); // Add this forward declaration
class Credit {
    double amount{};
    int clientID{};
    dtime take_date{};
    dtime return_date{};
    time_t recalcDate{};
	int startAmount{};
    int percent{};
public:
    Credit() = default;
    Credit(int amount, dtime take_date, dtime return_date, int percent,int clientID) : startAmount(amount), amount(amount),recalcDate(mktime(&ctimeToTm(take_date))), take_date(take_date), return_date(return_date), percent(percent), clientID(clientID) {}
    void showInfo() {
        cout << "Credit amount: " << amount << "\nTake date: " << take_date.year << "-" << take_date.month << "-" << take_date.day
            << " " << take_date.hour << ":" << take_date.minute << ":" << take_date.second
            << "\nReturn date: " << return_date.year << "-" << return_date.month << "-" << return_date.day
            << " " << return_date.hour << ":" << return_date.minute << ":" << return_date.second
            << "\nPercent: " << percent << "%" << endl
            << "\nClientID: " << clientID << endl;
    }
    int DecreaseCredit(int num) {
        if (num <= 0) return 0;
        amount -= num;
        if (amount <= 0) {
            amount = 0; // Ensure amount doesn't go negative
            cout << "Credit is fully paid off!\n";
            return 1; // credit paid off
        } else {
            cout << "Credit amount left: " << amount << endl;
            return 0; // credit still active
        }
    }
    int getClientID() { return clientID; }
    void recalcCredit() {
        time_t now = time(nullptr);  // поточний час
        double diffSeconds = difftime(now, recalcDate);
        int days = static_cast<int>(diffSeconds / (60 * 60 * 24));
        if (days > 0) {
            double rate = static_cast<double>(percent) / 100.0;
            //Нарахування складних відсотків
            amount += startAmount * rate * days;  // Фіксований відсоток від початкової суми
            // Оновлюємо recalcDate до початку поточного дня
            tm* nowTm = localtime(&now);
            nowTm->tm_hour = 0;
            nowTm->tm_min = 0;
            nowTm->tm_sec = 0;
            recalcDate = mktime(nowTm);  // зберігаємо оновлений час
        }
    }
    //вхід в функцію, розрахування різниці в днях, після цього якщо днів більше ніж 0 тоді нараховується кредит по кількості днів і під той відсоток
    //який потрібно і потім є рекалк дейт яка має дату останнього перерозрахування кредиту і таким чином  ми кожен раз міняємо тільки день
    //місяць рік але дату залишаємо меньшу по типу години хвилини і тп recalcDate на початку має дату взяття кредиту до першого рекалку де заміниться день
};
static vector<Credit> credits;
class Person {
    char name[STR_SIZE];
    char surname[STR_SIZE];
    char lastname[STR_SIZE];
    int PersonID=0;
public:
    Person() { name[0] = surname[0] = lastname[0] = 0;PersonID = 0; }

    Person(const char* n, const char* s, const char* l,int Pid) {
        strncpy(name, n, STR_SIZE); name[STR_SIZE - 1] = 0;
        strncpy(surname, s, STR_SIZE); surname[STR_SIZE - 1] = 0;
        strncpy(lastname, l, STR_SIZE); lastname[STR_SIZE - 1] = 0;
        PersonID = Pid;
    }
    Person(const Person& p) {
        strncpy(name, p.name, STR_SIZE);
        strncpy(surname, p.surname, STR_SIZE);
        strncpy(lastname, p.lastname, STR_SIZE);
        PersonID = p.PersonID;
    }
    void inputFromConsole() {
        cout << "Enter Name: "; cin >> name;
        cout << "Enter Surname: "; cin >> surname;
        cout << "Enter Lastname: "; cin >> lastname;
    }

    friend ostream& operator<<(ostream& os, Person& p) {
        return os << p.name << " " << p.surname << " " << p.lastname<<" "<<p.PersonID;
    }


    string getFullName() const { return string(name) + " " + string(surname) + " " + string(lastname); }
    string getName() const { return name; }
    string getSurname() const { return surname; }
    string getLastname() const { return lastname; }
    int getPersonID() { return PersonID; }
    void setPID(int num) {
        PersonID = num;
    }
};
class Position {
    int salary;
    char position[STR_SIZE];
public:
    Position() = default;
    void setSalary(int num) { salary = num; }
    void increaseSalary(int num) { salary += num; }
    void decreaseSalary(int num) { salary -= num; }
    int getSalary() { return salary; }

    void setPosition(const char* pos) { strcpy(position, pos); }
    string getPosition() { return position; }

    void showInfo() { cout << "Salary is: " << salary << "\nPosition is: " << position<<endl; }
};
class Worker : public Person {
    int ID = 0;
    int WorkBranchID = 0;
    int WorkBankID = 0;
    // positions storage removed; position history managed by WorkerPosada
    BankBranch* workBranch = nullptr;
public:
    Worker() : ID(0) {}
    Worker(const Person& p, int ID) : Person(p), ID(ID) {}
    Worker(const Worker& w) = default;
    void ShowInfo() {
        cout << "\n|-------------------|\nName: " << getName() \
            << "\nSurname: " << getSurname()\
            << "\nLastname: " << getLastname() \
            << "\nID: " << ID << endl;
    }
    bool findWorkBranch(vector<BankBranch>& branches);
    int getID() { return ID; }
    void setWorkBankID(int id) { WorkBankID = id; }
    int getWorkBankID() { return WorkBankID; }
    void setWorkBranchID(int id) { WorkBranchID = id; }
    int getWorkBranchID() { return WorkBranchID; }

    //menu where worker do his work
    void withdrawMoneyFromBranch(Client& client, int num, vector<BankBranch>& branches);
    void putMoneyToBranch(Client& client, int num);
    void giveCreditToBranch(Client& client, int amount, int percent, int termDays);
    // allow admin to change worker ID
    void setID(int id) { ID = id; }
    void setWorkBranch(BankBranch* branch);
};
class WorkerPosada : public Worker, public Position {
    dtime employDate{};
    dtime fireDate{};
public:
    WorkerPosada() = default;
    WorkerPosada(Worker& worker, const Position& position, const dtime& employDate)
        : Worker(worker), Position(position), employDate(employDate) {}

    void setFireDate(const dtime& fd) { fireDate = fd; }

    void showInfo() {
        cout << "\n|--- Position History ---|\n"
             << "Worker ID: " << getID() << "\nPosition: " << getPosition()
             << "\nSalary: " << getSalary()
             << "\nEmploy date: " << employDate.year << "-" << employDate.month << "-" << employDate.day
             << "\nFire date: "
             << "\nFull Name: "<<Person::getFullName();
        if (fireDate.year) cout << fireDate.year << "-" << fireDate.month << "-" << fireDate.day;
        else cout << "N/A";
        cout << endl;
    }

    int calculateTenure() const {
        tm employTm = ctimeToTm(employDate);
        time_t now = time(nullptr);
        tm fireTm = fireDate.year ? ctimeToTm(fireDate) : *localtime(&now);
        time_t employTime = mktime(&employTm);
        time_t fireTime = mktime(&fireTm);
        return static_cast<int>(difftime(fireTime, employTime) / (60 * 60 * 24)); // tenure in days
    }
};
class Client : public Person {
    int balance=0;
    int clientID=0;
    int bankID=0;
public:
    Client() = default;
    Client(Person& p, int clientID) : Person(p), clientID(clientID){}
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
    int bankID=0;
    int money = 0;
    char name[20] = {};
public:
    Bank() = default;
    Bank(int money, const char name[]) : money(money) { strcpy(this->name, name); }
    void showInfo() { cout << "\n|-------------------|\nBank name: " << name << "\nBank money: " << money<<endl; }
    void setMoney(int money) { this->money = money; }
    void setName(const char name[]) { strcpy(this->name, name); }
    void setBankID(int bankID) { this->bankID = bankID; }
    int getBankID() { return bankID; }

    void displayActualBranches(vector<BankBranch>& branches);
    int transferMoneyToBranch(BankBranch& branch, int num);
    int getAllBankMoney(vector<BankBranch>& branches);
    int getBranchesMoney(vector<BankBranch>& branches);
    vector<BankBranch*> getActualBranches(vector<BankBranch>& branches);
    int getBranchesCount(vector<BankBranch>& branches);
    void recalcAllMoneyFromBranches(vector<BankBranch>& branches);
    void WithdrawMoneyFromBranches(int num, vector<BankBranch>& branches);
    int WithdrawMoney(int num, vector<BankBranch>& branches, Client& client);
};
//через відділення банку можна звязувати з головним банком та запитувати доступ до операцій таких як покласти гроші зняти гроші кредит і депозит
//коротко кажучи саме через цю штуку будуть відбуватися всі операції
//на операції можуть робити запити робітники банку тим самим вони роблять реквест до відділення відділення звязується з банком і видає результат операції 
class Transaction {
    int amount{};
    dtime date{};
    char type[20]{};
    char from[20]{};
    char to[20]{};
    int clientID{};
    int workerID{};
public:
    Transaction() = default;
    Transaction(int amount, dtime date, const char* type, const char* from, const char* to, int clientID) {
        this->amount = amount;
        this->date = date;
        strncpy(this->type, type, 20); this->type[19] = '\0';
        strncpy(this->from, from, 20); this->from[19] = '\0';
        strncpy(this->to, to, 20); this->to[19] = '\0';
        this->clientID = clientID;
    }
    void showInfo() {
        cout << "Transaction: " << type
             << "\nAmount: " << amount
             << "\nDate: " << date.year << "-" << date.month << "-" << date.day
             << " " << date.hour << ":" << date.minute << ":" << date.second
             << "\nFrom: " << from
             << "\nTo: " << to
             << "\nClient ID: " << clientID << endl;
    }
    int getClientID() const { return clientID; }
};

static vector<Transaction> transactions;

// utility to get current dtime
static dtime getCurrentTime() {
    time_t now = time(nullptr);
    tm* lt = localtime(&now);
    return tmToCtime(*lt);
}

// show transactions for a client
void showClientTransactions(int clientID) {
    cout << "\n=== Transaction History ===\n";
    for (auto& t : transactions) {
        if (t.getClientID() == clientID) t.showInfo();
    }
    cout << "=== End of History ===\n";
}

class BankBranch {
    int branchNUM=0;
    char addr[50] = {};
    int bankID = 0;
    int money=0;
    Bank* bank_ptr = nullptr;
public:
    BankBranch() = default;
    BankBranch(int bankID, int branchNUM, const char* addr) : bankID(bankID), branchNUM(branchNUM) { strcpy(this->addr, addr); }

    int getMoneyFromBank(int num) { int responce = bank_ptr->transferMoneyToBranch(*this, num); if (responce == 0) { cout << "Transfered succesfully!\n"; return 0; } else if (responce == 1) { cout << "Bank dont have enough money!\n";return 1; } else return 1; }
    void setBankID(int bankID) { this->bankID = bankID; }
    int getBranchID() { return bankID; }
    int getBankID() { return bankID; }
    void setBankPointer(Bank* b) { bank_ptr = b; }
    void setBranchMoney(int num) { money = num; }
    int getBranchMoney() { return money; }
    int getBranchNum() { return branchNUM; }
    void decreaseBranchMoney(int num) { money -= num; }
    void increaseBranchMoney(int num) { money += num; }
    void displayBranchInfo() { cout << "\nBranch bankID: " << bankID << "\nBranch money: " << money<<"\nBranch address: "<<addr<<"\nBranch num: "<<branchNUM; }
    friend ostream& operator<<(ostream& os, const BankBranch& b) {
        return os << "Branch bankID: " << b.bankID << "\nBranch money: " << b.money << "\nBranch address: " << b.addr << "\nBranch num: " << b.branchNUM;
    }

    int WithdrawMoney(int num, Client& client, vector<BankBranch>& branches) {
        if (client.getClientBalance() <= num)
            cout << "You dont have enough money";
        else {
            if (num < getBranchMoney()) {
                client.decreaseClientBalance(num);setBranchMoney(getBranchMoney() - num);
                dtime dt = getCurrentTime();
                char from_str[20], to_str[20];
                snprintf(from_str, 20, "branch%d", branchNUM);
                snprintf(to_str, 20, "client%d", client.getClientID());
                transactions.emplace_back(num, dt, "withdraw", from_str, to_str, client.getClientID());
            }
            else {
                if (bank_ptr->getAllBankMoney(branches) > num) {
                    int reposnse=0;
                    if (reposnse=bank_ptr->WithdrawMoney(num, branches, client) == 1)return 1;
					else if (reposnse == 0) {
                        client.decreaseClientBalance(num); // Тільки тут знімаємо з клієнта
						setBranchMoney(getBranchMoney() - num);
					}
					else cout << "Cannot withdraw: insufficient funds.\n";
                    dtime dt = getCurrentTime();
                    char from_str[20], to_str[20];
                    snprintf(from_str, 20, "bank%d", bankID);
                    snprintf(to_str, 20, "client%d", client.getClientID());
                    transactions.emplace_back(num, dt, "withdraw", from_str, to_str, client.getClientID());
                }
            }
        } return 0;
    }
    //void PutMoney(int num, Client& cl, vector<BankBranch>& branches) { if (cl.getClientBalance() < num)cout << "You have no money!\n";else { if (num > this->money && num <= bank_ptr->getAllBankMoney(branches)); } }
    void PutMoney(int num, Client& client) { client.increaseClientBalance(num);increaseBranchMoney(num); // record transaction
        dtime dt = getCurrentTime();
        char from_str[20], to_str[20];
        snprintf(from_str, 20, "client%d", client.getClientID());
        snprintf(to_str, 20, "branch%d", branchNUM);
        transactions.emplace_back(num, dt, "deposit", from_str, to_str, client.getClientID()); }
    void GiveCredit(int amount, Client& client, int percent, int termDays) {
        // grant credit
        dtime take = getCurrentTime();
        time_t tt = time(nullptr) + termDays * 86400;
        dtime ret = tmToCtime(*localtime(&tt));
        client.increaseClientBalance(amount);
        credits.emplace_back(amount, take, ret, percent,client.getClientID());
        //доробити конструктор і сохраняти ClientID
        // record transaction
        char from_str[20], to_str[20];
        snprintf(from_str,20,"branch%d",branchNUM);
        snprintf(to_str,20,"client%d",client.getClientID());
        transactions.emplace_back(amount, take, "credit", from_str, to_str, client.getClientID());
    }

};
int Bank::WithdrawMoney(int num, vector<BankBranch>& branches, Client& cl) {
    recalcAllMoneyFromBranches(branches);
    if (money >= num) {
        money -= num;
    }
    else if (getAllBankMoney(branches) > num) {
        WithdrawMoneyFromBranches(num, branches);
        money -= num;
    }
    else {
        return 1; // Недостатньо коштів
    }
    return 0;
}int Bank::transferMoneyToBranch(BankBranch& branch, int num) { if (num <= money) { money -= num;branch.increaseBranchMoney(num);return 0; } else return 1; }
int Bank::getAllBankMoney(vector<BankBranch>& branches) { int sum = money;vector<BankBranch*>temp = getActualBranches(branches);for (auto& a : temp)sum += a->getBranchMoney(); return sum; }
int Bank::getBranchesMoney(vector<BankBranch>& branches) { int sum = 0;vector<BankBranch*>temp = getActualBranches(branches);for (auto& a : temp)sum += a->getBranchMoney(); return sum; }
vector<BankBranch*> Bank::getActualBranches(vector<BankBranch>& branches) { vector<BankBranch*> tempBranches;for (auto& a : branches)if (a.getBranchID() == bankID)tempBranches.push_back(&a);return tempBranches; }
int Bank::getBranchesCount(vector<BankBranch>& branches) { int count = 0;for (auto& a : branches)if (a.getBranchID() == bankID)count++;return count; }
void Bank::recalcAllMoneyFromBranches(vector<BankBranch>& branches) { vector<BankBranch*> tempBranches = getActualBranches(branches);int sum = getBranchesMoney(branches);int count = getBranchesCount(branches);if (count > 0) { for (auto& a : tempBranches)a->setBranchMoney(sum / count);tempBranches.clear(); } else cout << "There is zero branches"; }
void Bank::WithdrawMoneyFromBranches(int num, vector<BankBranch>& branches) { vector<BankBranch*> tempbranch = getActualBranches(branches);int count = getBranchesCount(branches), sum = getBranchesMoney(branches), avg = sum / count;for (auto& a : tempbranch) { money += avg;a->decreaseBranchMoney(avg); } }
void Bank::displayActualBranches(vector<BankBranch>& branches) { vector<BankBranch*> tempBranches = getActualBranches(branches);int count = 0;cout<<"Actual branches: "; for (auto& a : tempBranches) { count++;cout <<endl<< count << ')'; a->displayBranchInfo(); }cout << "\n|-------------------|\n"; }

bool Worker::findWorkBranch(vector<BankBranch>& branches) { for (auto& a : branches)if (a.getBranchID() == WorkBranchID && a.getBankID() == WorkBankID) { workBranch = &a;return true; }return false; }

void Worker::putMoneyToBranch(Client& client, int num) { workBranch->PutMoney(num, client); }
void Worker::withdrawMoneyFromBranch(Client& client, int num, vector<BankBranch>& branches) { if (workBranch->WithdrawMoney(num, client, branches) == 1)cout << "Please top up your account!\n"; }
void Worker::giveCreditToBranch(Client& client, int amount, int percent, int termDays) { workBranch->GiveCredit(amount, client, percent, termDays); }
void Worker::setWorkBranch(BankBranch * branch) { workBranch = branch;WorkBranchID = branch->getBranchID();WorkBankID = branch->getBankID(); }
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
    for (auto& w : wk) {
        if (w.getID() == id) {
            return false;
        }
    }
    return true;
}
bool isBranchNumUnique(int num, int BankID, vector<BankBranch>& branches) {
    for (auto& a : branches)if (num == a.getBranchNum() && BankID == a.getBranchID())return false;
    return true;
}
bool isPersonIDUnique(int num, vector<Person>& p) { for (auto& a : p)if (a.getPersonID() == num)return false;return true; }
bool isClientIDUnique(int num, vector<Client>& p) { for (auto& a : p)if (a.getClientID() == num)return false;return true; }
bool isLoginUnique(const char* login, vector<Account>& p) { for (auto& a : p)if (a.getLogin() == (string)login)return false;return true; }

//функція для знаходження актуального вказівника на головний банк по айді
void linkBank(vector<BankBranch>& branches, Bank& bank) {
    for (auto& branch : branches) {
        branch.setBankPointer(&bank);
    }
}
dtime inputCtime() {
    dtime ct;
    cout << "Введіть рік: "; cin >> ct.year;
    cout << "Введіть місяць (1-12): "; cin >> ct.month;
    cout << "Введіть день (1-31): "; cin >> ct.day;
    cout << "Введіть годину (0-23): "; cin >> ct.hour;
    cout << "Введіть хвилину (0-59): "; cin >> ct.minute;
    cout << "Введіть секунду (0-59): "; cin >> ct.second;
    return ct;
}
dtime tmToCtime(const tm& timeinfo) {
    dtime result;
    result.year = timeinfo.tm_year + 1900;
    result.month = timeinfo.tm_mon + 1;
    result.day = timeinfo.tm_mday;
    result.hour = timeinfo.tm_hour;
    result.minute = timeinfo.tm_min;
    result.second = timeinfo.tm_sec;
    return result;
}
tm ctimeToTm(const dtime& ct) {
    tm result = {};
    result.tm_year = ct.year - 1900;
    result.tm_mon = ct.month - 1;
    result.tm_mday = ct.day;
    result.tm_hour = ct.hour;
    result.tm_min = ct.minute;
    result.tm_sec = ct.second;
    return result;
}




















// Admin submenu for managing people
int AdminPeopleMenu(vector<Person>& person) {
    while (true) {
        cout << "\n--- People Menu ---\n"
             << "1) List people\n"
             << "2) Add person\n"
             << "3) Edit person\n"
             << "4) Remove person\n"
             << "5) Back\n"
             << "Enter: "; int ch; cin >> ch;
        if (ch == 1) {
            for (auto& p : person) cout << p << endl;
        } else if (ch == 2) {
            Person np; np.inputFromConsole();while (true) { cout << "Enter PersonID: ";int pid;cin >> pid;if (isPersonIDUnique(pid, person)) { np.setPID(pid);break; } else cout << "Enter another PersonID!\n"; }
            person.push_back(np);
        } else if (ch == 3) {
            cout << "Index to edit: "; int i; cin >> i;
            if (i >= 1 && i <= (int)person.size()) {
                person[i-1].inputFromConsole();
            } else {
            }
        } else if (ch == 4) {
            int g = 1;
            for (auto& p : person) cout << g++ << ')' << p << endl;
            cout << "Index to remove: "; int i; cin >> i;
            if (i >= 1 && i <= (int)person.size()) {
                person.erase(person.begin() + i - 1);
            } else {
            }
        } else if (ch == 5) {
            break;
        } else {
            cout << "Wrong choice!\n";
        }
    }
    return 0;
}

// Admin submenu for managing workers
// Now selects existing Person to become a worker with position and hire date
int AdminWorkersMenu(vector<Person>& person, vector<Worker>& workers, Bank& bank, vector<BankBranch>& branches, vector<WorkerPosada>& wposada) {
    while (true) {
        cout << "\n--- Workers Menu ---\n"
             << "1) List workers\n"
             << "2) Add worker\n"
             << "3) Remove worker\n"
             << "4) Fire worker\n"
             << "5) Reassign worker\n"
             << "6) View position history\n"
             << "7) Back\n"
             << "Enter: "; int ch; cin >> ch;
        if (ch == 1) {
            for (auto& w : workers) w.ShowInfo();
        } else if (ch == 2) {
            // ensure at least one branch exists, offer to create
            while (branches.empty()) {
                cout << "No branches available. Create branch now? (1-Yes, 2-No): "; int opt; cin >> opt;
                if (opt == 1) AdminBranchesMenu(branches, bank);
                else break;
            }
            if (branches.empty()) continue;
            // select existing person
            cout << "Select person to make worker:\n";
            int i = 0;
            for (auto& a : person)
            {
                cout << i + 1 << ") " << a << "\n";
                ++i;
            }
            int pi; cin >> pi;
            if (pi < 1 || pi > (int)person.size()) continue;
            Person& p = person[pi-1];
            int id;
            while (true) {
                int num = 1 + std::rand() % 100000;
                if (isIDUnique(num, workers)) {
                    id = num;
                    break;
                }
            }
            cout << "Enter position name: "; string posName; cin >> posName;
            cout << "Enter salary: "; int sal; cin >> sal;
            // enter hire date
            cout << "Enter hire date:\n";
            dtime hireDate = inputCtime();
            Position pos; pos.setPosition(posName.c_str()); pos.setSalary(sal);
            //воркера має право створити тільки адміністратор тому що як інакше , таким чином все можна легко організувати
            Worker w(p, id);
            if (!branches.empty()) {
                cout << "Select branch number:\n";
                for (int i = 0; i < (int)branches.size(); ++i)
                    cout << i+1 << ") " << branches[i] << "\n";
                int b; cin >> b;
                if (b >= 1 && b <= (int)branches.size()) {
                    w.setWorkBankID(branches[b-1].getBankID());
                    w.setWorkBranchID(branches[b-1].getBranchNum());
                    w.findWorkBranch(branches);
                }
            }
            workers.push_back(w);
            // record position history
            wposada.emplace_back(w, pos, hireDate);
        } else if (ch == 3) {
            int g = 1;
            for (auto& p : workers) cout << g++ << ')' << p << endl;
            cout << "Index to remove: "; int i; cin >> i;
            if (i >= 1 && i <= (int)workers.size()) {
                workers.erase(workers.begin() + i - 1);
                // also remove related position histories
                wposada.erase(remove_if(wposada.begin(), wposada.end(), [&](WorkerPosada& wp){ return wp.getID() == workers[i-1].getID(); }), wposada.end());
            }
        } else if (ch == 4) {
            cout << "Select worker to fire:\n";
            for (int i = 0; i < (int)workers.size(); ++i)
                cout << i+1 << ") " << workers[i].getFullName() << "\n";
            int wi; cin >> wi;
            if (wi < 1 || wi > (int)workers.size()) continue;
            Worker& w = workers[wi-1];
            dtime fireDate = getCurrentTime();
            for (auto& wp : wposada) {
                if (wp.getID() == w.getID()) {
                    wp.setFireDate(fireDate);
                    cout << "Worker fired successfully.\n";
                    break;
                }
            }
        } else if (ch == 5) {
            cout << "Select worker to reassign:\n";
            for (int i = 0; i < (int)workers.size(); ++i)
                cout << i+1 << ") " << workers[i].getFullName() << "\n";
            int wi; cin >> wi;
            if (wi < 1 || wi > (int)workers.size()) continue;
            Worker& w = workers[wi-1];
            cout << "Enter new position name: "; string posName; cin >> posName;
            cout << "Enter new salary: "; int sal; cin >> sal;
            dtime hireDate = getCurrentTime();
            Position newPos; newPos.setPosition(posName.c_str()); newPos.setSalary(sal);
            wposada.emplace_back(w, newPos, hireDate);
            cout << "Worker reassigned successfully.\n";
        } else if (ch == 6) {
            cout << "Select worker to view position history:\n";
            for (int i = 0; i < (int)workers.size(); ++i)
                cout << i+1 << ") " << workers[i].getFullName() << "\n";
            int wi; cin >> wi;
            if (wi < 1 || wi > (int)workers.size()) continue;
            Worker& w = workers[wi-1];
            cout << "\n=== Position History for " << w.getFullName() << " ===\n";
            for (auto& wp : wposada) {
                if (wp.getID() == w.getID()) {
                    wp.showInfo();
                    cout << "Tenure: " << wp.calculateTenure() << " days\n";
                }
            }
            cout << "=== End of History ===\n";
        } else if (ch == 7) {
            break;
        } else {
            cout << "Wrong choice!\n";
        }
    }
    return 0;
}

// Admin submenu for managing clients (select from existing persons)
int AdminClientsMenu(vector<Person>& person, vector<Client>& clients) {
    while (true) {
        cout << "\n--- Clients Menu ---\n"
             << "1) List clients\n"
             << "2) Add client (select existing person)\n"
             << "3) Remove client\n"
             << "4) Back\n"
             << "Enter: "; int ch; cin >> ch;
        if (ch == 1) {
            for (auto& c : clients) c.ShowInfo();
        } else if (ch == 2) {
            // select existing person for client
            cout << "Select person to become client:\n";
            for (int i = 0; i < (int)person.size(); ++i)
                cout << i+1 << ") " << person[i] << "\n";
            int pi; cin >> pi;
            if (pi < 1 || pi > (int)person.size()) continue;
            cout << "Enter client ID: "; int id; cin >> id;
            clients.emplace_back(person[pi-1], id);
        } else if (ch == 3) {
            int g = 1;
            for (auto& p : clients) cout << g++ << ')' << p << endl;
            cout << "Index to remove: "; int i; cin >> i;
            if (i >= 1 && i <= (int)clients.size()) clients.erase(clients.begin() + i - 1);
        } else if (ch == 4) {
            break;
        } else cout << "Wrong choice!\n";
    }
    return 0;
}

// Admin submenu for managing bank
int AdminBanksMenu(Bank& bank) {
    while (true) {
        cout << "\n--- Banks Menu ---\n"
             << "1) Show bank info\n"
             << "2) Set bank name\n"
             << "3) Set bank money\n"
             << "4) Back\n"
             << "Enter: "; int ch; cin >> ch;
        if (ch == 1) bank.showInfo();
        else if (ch == 2) {
            cout << "Enter bank name: "; char m[20]; cin >> m;
            bank.setName(m);
        } else if (ch == 3) {
            cout << "Enter bank money: "; int m; cin >> m;
            bank.setMoney(m);
        }
        else if (ch == 4) {
            break;
        }
        else cout << "Wrong choice!\n";
    }
    return 0;
}

// Admin submenu for managing branches
int AdminBranchesMenu(vector<BankBranch>& branches, Bank& bank) {
    while (true) {
        cout << "\n--- Branches Menu ---\n"
             << "1) List branches\n"
             << "2) Add branch\n"
             << "3) Remove branch\n"
             << "4) Back\n"
             << "Enter: "; int ch; cin >> ch;
        if (ch == 1) {
            for (auto& b : branches) b.displayBranchInfo();
        } else if (ch == 2) {
            cout << "Enter branch address: ";char addr[50];cin.ignore();cin.getline(addr, 50);int branchNum;
            while (true) {
                cout << "Enter branch number: ";
                cin >> branchNum;
                if (isBranchNumUnique(branchNum, bank.getBankID(), branches)) {
                    break;
                }
                else {
                    cout << "Branch number already exists in this bank!\n";
                }
            }
            BankBranch br(bank.getBankID(), branchNum, addr);
            br.setBankPointer(&bank);
            while (true) {
                int money;
                cout << "Enter amount of money to transfer from main: ";
                cin >> money;
                int response = br.getMoneyFromBank(money);
                if (response == 0) {
                    branches.push_back(br);
                    cout << "Branch created and linked to bank ID: " << br.getBranchID() << endl;
                    break;
                }
                else if (response == 1) {
                    cout << "Enter smaller amount of money, bank doesn't have enough!\n";
                }
            }
        } else if (ch == 3) {
            int g = 1;
            if (!branches.empty())for (auto& p : branches) cout << g++ << ')' << p << endl;else cout << "There is no branches!\n";
            cout << "Index to remove: "; int i; cin >> i;
            if (i >= 1 && i <= (int)branches.size()) branches.erase(branches.begin() + i - 1);
        } else if (ch == 4) {
            break;
        } else cout << "Wrong choice!\n";
    }
    return 0;
}

// invoke updated workers menu
int AdminMenu(
    vector<Person>& person,
    vector<Worker>& workers,
    vector<Client>& clients,
    Bank& bank,
    vector<BankBranch>& branches,
    vector<WorkerPosada>& wposada,
	vector<Account>& accounts
) {
    while (true) {
        cout << "\n=== ADMIN MENU ===\n"
             << "1) people menu\n"
             << "2) workers menu\n"
             << "3) clients menu\n"
             << "4) banks menu\n"
             << "5) branches menu\n"
             << "6) Exit menu\n"
             << "7) Enter new TelegramID\n"
             << "8) Account management\n"
             << "Enter: "; int choice; cin >> choice;
        if (choice == 1) {
            AdminPeopleMenu(person);
        } else if (choice == 2) {
            AdminWorkersMenu(person, workers, bank, branches, wposada);
        } else if (choice == 3) {
            AdminClientsMenu(person, clients);
        } else if (choice == 4) {
            AdminBanksMenu(bank);
        } else if (choice == 5) {
            AdminBranchesMenu(branches, bank);
        } else if (choice == 6) {
            break;
        } else if (choice == 7) {
            cout << "1)List accounts\n2)Add account\n3)Remove account\n4)Change passord\n5)Exit\nEnter: "; int ch; cin >> ch;
            if (ch == 1) {
                for (auto& a : accounts) cout << a << endl;
            } else if (ch == 2) {
                char login[20], password[20], role[20];
                cout << "Enter login: "; cin >> login;
                cout << "Enter password: "; cin >> password;
                cout << "Enter role (admin/worker/user): "; cin >> role;
                int personID;
                cout << "Enter PersonID: "; cin >> personID;
                Account newAcc(login, password, role, personID);
                accounts.push_back(newAcc);
            } else if (ch == 3) {
                int index; cout << "Enter index to remove: "; cin >> index;
                if (index >= 1 && index <= (int)accounts.size()) {
                    accounts.erase(accounts.begin() + index - 1);
                    cout << "Account removed successfully.\n";
                } else {
                    cout << "Invalid index!\n";
                }
            } else if (ch == 4) {
                char login[20], newPassword[20];
                cout << "Enter login: "; cin >> login;
                cout << "Enter new password: "; cin >> newPassword;
                bool found = false;
                for (auto& a : accounts) {
                    if (strcmp(a.getLogin(), login) == 0) {
                        a.setPassword(newPassword);
                        found = true;
                        cout << "Password changed successfully.\n";
                        break;
                    }
                }
                if (!found) {
                    cout << "Account with this login not found!\n";
                }
            } else {
                cout << "Wrong choice!\n";
            }
        }
        else {
            cout << "Wrong choice!\n";
        }
    }
    return 0;
}

int WorkerMenu(vector<Client>& clients, Bank &bank, vector<BankBranch>& branches, vector<Worker>& workers, vector<WorkerPosada>&wposada, vector<Person>& person,vector<Account>&accounts);
// User menu with clients, bank, branches and workers
int UserMenu(vector<Client>& clients, Bank &bank, vector<BankBranch>& branches, vector<Worker>& workers);
int LoginMenu(
    vector<Person>&person,
    vector<Worker>&workers,
    vector<Client>&clients,
    Bank &bank,
    vector<BankBranch>&branches,
    vector<WorkerPosada>&wposada,
	vector<Account>& accounts
) {
    cout << "Welcome to the bank system!\n";
    cout << "1)Login\n2)Register\n3)Exit\nEnter: ";int choice; cin >> choice;
    if (choice == 1) {
        while (true) {//вхід в аккаунт за роллю
            bool found = false;
            char login[20], password[20];
            cout << "Now enter login: "; cin >> login;
            cout << "Now enter password: "; cin >> password;
            for (auto& a : accounts) {
                if (strcmp(a.getLogin(), login) == 0 && strcmp(a.getPassword(), password) == 0) {
                    found = true;accPtr = &a;
                    if (a.getRole() == string("admin")) {
                        AdminMenu(person, workers, clients, bank, branches, wposada,accounts);
                    } else if (a.getRole() == string("worker")) {
                        WorkerMenu(clients, bank, branches, workers,wposada,person,accounts);
                    } else if (a.getRole() == string("user")) {
                        UserMenu(clients, bank, branches, workers);
                    }
                    break;
                }
            }
            if (found) break;
            else { cout << "Enter correct login & password!\n"; }
        }

    } else if (choice == 2) {
        bool logged = false;
        string chat_id;
        string token="8101900575:AAFCC1UxZAyN946qcoeEvZhXM7VuE_UhHNU";
        while(true) {
            if (logged) { LoginMenu(person, workers, clients, bank, branches, wposada,accounts);break; }
            cout << "1)Register in system\n2)Exit\nEnter choice: "; int ch; cin >> ch;
            if(ch==1){
                cout << "Are you registered in bot?\n1)Yes\n2)No\n3)Exit\nEnter: ";int rees; cin >> rees;
                if (rees == 1) {
                    int randNum = rand() % RAND_MAX;
                    char text[20];
                    sprintf(text, "%d", randNum);
                    while (true) {
                        cout << "Enter your telegram id: ";cin >> chat_id;bool found = false;//tyt
                        for (auto& a : accounts) { if (a.getTelegramID() == chat_id) { found = true;accPtr = &a;break; } }
                        if (found) {
                            for (auto& a : person)if (a.getPersonID() == accPtr->getPersonID())cout << "You registered in systeam as: " << a<<endl<<"please login!\n";
                            break;
                        }
                        else {
                            string cmd = "curl -s -X POST \"https://api.telegram.org/bot" +
                                string(token) + "/sendMessage\" -d \"chat_id=" +
                                chat_id + "\" -d \"text=" + text + "\" >nul 2>&1";
                            system(cmd.c_str());
                            cout << "Enter the code what sended on your telegram account: ";int num;cin >> num;if (num == randNum)cout << "You verificated now can continue registration!\n";
                            char login[20];
                            while (true) {
                                cout << "Enter login: ";cin >> login;if (isLoginUnique(login, accounts))break;cout << "Enter another login!\n";
                            }
                            cout << "Enter password: ";char password[20];cin >> password;
                            Person np; np.inputFromConsole();
                            while (true) {
                                int num = 1 + std::rand() % 100000;
                                if (isPersonIDUnique(num, person)) { np.setPID(num);break; }
                            }
                            int pid = np.getPersonID();person.push_back(np);
                            Account newUserAcc(login, password, "user", pid);newUserAcc.setTelegramID(chat_id);newUserAcc.setVerified(true);accounts.push_back(newUserAcc);
                            while (true) {
                                int num = 1 + std::rand() % 100000;
                                if (isClientIDUnique(num, clients)) { Client newClient(np, num);clients.push_back(newClient);break; }
                            }
                            cout << "Succesfully registered!\n";logged = true;
                            cout << "Returning to login menu\n";
                            LoginMenu(person, workers, clients, bank, branches, wposada,accounts);
                            break;
                        }
                        if (logged)break;
                    }
                }
                else if (rees == 2) {
                    cout << "If your ID entered in Database by admin press start in @DeFexDownloader_Bot\nElse get OUT!!!\n";continue;
                }
                else if (rees == 3)break;
            }
            else if (ch == 3) { LoginMenu(person, workers, clients, bank, branches, wposada,accounts); }
        }
    }
        return 0;
}





int WorkerMenu(vector<Client>& clients, Bank& bank, vector<BankBranch>& branches, vector<Worker>& workers,vector<WorkerPosada>& wposada,vector<Person>& person,vector<Account>&accounts) {
    //|----------------------Identity----------------------|
    Worker* curW=NULL;
    for (auto& a : workers) { if (accPtr->getPersonID() == a.getPersonID())curW = &a; }
    cout << "You are logged as: " << curW->getFullName() << endl;
    //|----------------------Identity----------------------|
    while (true) {
        cout << "\n=== WORKER MENU ===\n"
            << "1) Show clients\n"
            << "2) Show branches\n"
            << "3) Show bank info\n"
            << "4) Show my work history\n"
            << "5) Client interactions\n"
            << "6) Register new client\n"
            << "7) Exit\n"
            << "Enter: "; int choice; cin >> choice;
        if (choice == 1) {
            for (auto& c : clients) c.ShowInfo();
        }
        else if (choice == 2) {
            for (auto& b : branches) b.displayBranchInfo();
        }
        else if (choice == 3) {
            bank.showInfo();
        }
        else if (choice == 4) {
            cout << "\n=== Your Work History ===\n";
            for (auto& wp : wposada) {
                if (wp.getID() == curW->getID()) {
                    wp.showInfo();
                    cout << "Tenure: " << wp.calculateTenure() << " days\n";
                }
            }
            cout << "=== End of History ===\n";
        }
        else if (choice == 5) {
            cout << "Select client:\n";
            for (int i = 0; i < clients.size(); ++i)
                cout << i + 1 << ") " << clients[i].getFullName() << "\n";
            int ci; cin >> ci;
            if (ci < 1 || ci > clients.size()) continue;
            Client& cl = clients[ci - 1];
            while (true) {
                cout << "\n=== Client Interactions ===\n"
                    << "1) Show client transactions\n"
                    << "2) Take credit for client\n"
                    << "3) Repay credit for client\n"
                    << "4) Show client credits\n"
                    << "5) Show client balance\n"
                    << "6) Show client info\n"
                    << "7) Exit\n"
                    << "Enter: "; int ch; cin >> ch;

                if (ch == 1) {
                    showClientTransactions(cl.getClientID());
                }
                else if (ch == 2) {
                    cout << "Credit amount: "; int amt; cin >> amt;
                    cout << "Percent rate (%): "; int pct; cin >> pct;
                    cout << "Term days: "; int days; cin >> days;
                    curW->giveCreditToBranch(cl, amt, pct, days);
                    cout << "Credit taken successfully.\n";
                }
                else if (ch == 3) {
                    bool found = false;
					cout << "Enter amount to repay: "; int repayAmt; cin >> repayAmt;
                    for (auto& c : credits) {
                        if (c.getClientID() == cl.getClientID()) {
                            found = true;
							if (c.DecreaseCredit(repayAmt) == 1) // delete credit object from vector
							{
								cout << "Credit fully repaid and removed.\n";
								credits.erase(remove(credits.begin(), credits.end(), c), credits.end());
							}
							cl.decreaseClientBalance(repayAmt);
							dtime dt = getCurrentTime();
							transactions.emplace_back(repayAmt, dt, "repay", "worker", "bank", cl.getClientID());
                            break;
                        }
                    }
                    if (!found) {
                        cout << "Credit not found for this client.\n";
                    }
                }
                else if (ch == 4) {
                    for (auto& c : credits) {
                        if (c.getClientID() == cl.getClientID()) {
                            c.showInfo();
                        }
                    }
                }
                else if (ch == 5) {
                    cout << "Client balance: " << cl.getClientBalance() << "\n";
                }
                else if (ch == 6) {
                    cl.ShowInfo();
                }
                else if (ch == 7) {
                    showClientTransactions(cl.getClientID());
                }
            }
        }
            
		else if (choice == 6) { // Register new client
            string chat_id;
            string token = "8101900575:AAFCC1UxZAyN946qcoeEvZhXM7VuE_UhHNU";
			cout << "Enter client Telegram ID: "; cin >> chat_id;
			bool found = false;
			for (auto& a : accounts) { if (a.getTelegramID() == chat_id) { found = true; accPtr = &a; break; } }
			if (found) {
				for (auto& a : person) if (a.getPersonID() == accPtr->getPersonID()) cout << "Client registered in system as: " << a << endl << "please login!\n";
				continue;
            }
            else {
				string cmd = "curl -s -X POST \"https://api.telegram.org/bot" +
					string(token) + "/sendMessage\" -d \"chat_id=" +
					chat_id + "\" -d \"text=Please register in the bank system!\" >nul 2>&1";
				system(cmd.c_str());
				cout << "A message has been sent to your Telegram account. Please register there.\n";
            }
			char login[20];
			while (true) {
				cout << "Enter login: "; cin >> login;
				if (isLoginUnique(login, accounts)) break;
				cout << "Login already exists. Enter another login!\n";
			}
			cout << "Enter password: "; char password[20]; cin >> password;

            Person np; np.inputFromConsole();
            while (true) {
                int num = 1 + std::rand() % 100000;
                if (isPersonIDUnique(num, person)) { np.setPID(num);break; }
            }
            int pid = np.getPersonID();person.push_back(np);
            Account newUserAcc(login, password, "user", pid);newUserAcc.setTelegramID(chat_id);newUserAcc.setVerified(true);accounts.push_back(newUserAcc);
            while (true) {
                int num = 1 + std::rand() % 100000;
                if (isClientIDUnique(num, clients)) { Client newClient(np, num);clients.push_back(newClient);break; }
            }
			cout << "Enter client ID: "; int id; cin >> id;
			while (true) {
				if (isClientIDUnique(id, clients)) break;
				cout << "Client ID already exists. Enter another ID: "; cin >> id;
			}
			cout << "New client registered successfully.\n";
		}
		else if (choice == 7) {
            break;
        }
        else { cout << "Wrong choice!\n"; }
    }
    return 0;
}



int UserMenu(vector<Client>& clients,
                 Bank& bank,
                 vector<BankBranch>& branches,
                 vector<Worker>& workers) {
    //|----------------------Identity----------------------|
    Client* curCl{};
    for (auto& a : clients)if (a.getPersonID() == accPtr->getPersonID())curCl = &a;
    cout << "You are logged as: " << *curCl << std::endl;

    // Select branch
    cout << "Select branch:\n";
    for (int i = 0; i < branches.size(); ++i)
        cout << i+1 << ") " << branches[i] << "\n";
    int bi; cin >> bi;
    if (bi < 1 || bi > branches.size()) { cout << "Invalid branch\n"; return 1; }
    BankBranch& br = branches[bi-1];

    // Select worker at branch
    vector<Worker*> bw;
    for (auto& w : workers)
        if (w.getWorkBankID() == br.getBankID() && w.getWorkBranchID() == br.getBranchID()) {
            int b;
            if (b=strcmp(w.getFullName().c_str(), curCl->getFullName().c_str()) != 0)
                bw.push_back(&w);
        }
    if (bw.empty()) { cout << "No worker available at this branch.\n"; return 1; }
    cout << "Select worker:\n";
    for (int i = 0; i < bw.size(); ++i)
        cout << i+1 << ") " << bw[i]->getName() << " " << bw[i]->getSurname() << "\n";
    int wi; cin >> wi;
    if (wi < 1 || wi > bw.size()) { cout << "Invalid worker\n"; return 1; }
    Worker* serv = bw[wi-1];
    // link the worker to its branch if not already linked
    if (!serv->findWorkBranch(branches)) { cout << "Error: Worker not linked to branch\n"; return 1; }

    //|----------------------User Operations---------------------|
    while (true) {
        cout << "\n=== USER MENU ===\n"
             << "1) Show balance\n"
             << "2) Withdraw money\n"
             << "3) Deposit money\n"
             << "4) Show transaction history\n"
             << "5) Take credit\n"
             << "6) Repay credit\n"
             << "7) Show credit status\n"
             << "8) Exit\n"
             << "Enter: "; int choice; cin >> choice;
        if (choice == 1) {
            cout << "Your balance: " << curCl->getClientBalance() << "\n";
        } else if (choice == 2) {
            cout << "Amount to withdraw: "; int amt; cin >> amt;
            serv->withdrawMoneyFromBranch(*curCl, amt, branches);
        } else if (choice == 3) {
            cout << "Amount to deposit: "; int amt; cin >> amt;
            serv->putMoneyToBranch(*curCl, amt);
        } else if (choice == 4) {
            showClientTransactions(curCl->getClientID());
        } else if (choice == 5) {
            cout << "Credit amount: "; int amt; cin >> amt;
            cout << "Percent rate (%): "; int pct; cin >> pct;
            cout << "Term days: "; int days; cin >> days;
            serv->giveCreditToBranch(*curCl, amt, pct, days);
        } else if (choice == 6) {
            cout << "Amount to repay: "; int repayAmt; cin >> repayAmt;
            if (curCl->getClientBalance() >= repayAmt) {
                curCl->decreaseClientBalance(repayAmt);
                dtime dt = getCurrentTime();
                transactions.emplace_back(repayAmt, dt, "repay", "client", "bank", curCl->getClientID());
                bool creditPaidOff = false;
                for (auto it = credits.begin(); it != credits.end(); ++it) {
                    if (it->getClientID() == curCl->getClientID()) {
                        creditPaidOff = it->DecreaseCredit(repayAmt) == 1;
                        if (creditPaidOff) {
                            credits.erase(it);
                            cout << "Credit successfully paid off!\n";
                        }
                        break;
                    }
                }
                if (!creditPaidOff) {
                    cout << "Partial repayment processed.\n";
                }
            } else {
                cout << "Insufficient balance to repay.\n";
            }
        } else if (choice == 7) {
            cout << "\n=== Your Credits ===\n";
            for (auto& cr : credits) cr.showInfo();
            cout << "=== End of Credits ===\n";
        } else if (choice == 8) {
            break;
        } else {
            cout << "Wrong choice!\n";
        }
    }
    //|----------------------End User Operations---------------------|
    return 0;
}


















void linkWorkers(vector<BankBranch>& branches, vector<Worker>& workers) {
    for (auto& a : workers) {
        bool found = false;
        for (auto& b : branches) {
            if (a.getWorkBankID() == b.getBankID() && a.getWorkBranchID() == b.getBranchNum()) {
                a.setWorkBranch(&b);found = true;
            }
        }if(!found) {
            cout << "\nWorker not found in any branch!\nLink worker to another branch!\n";int num = 0;
            for(auto&a:branches){
                cout << num<<')'<< a;
            }cout << "Enter branch number: ";cin >> num;
            if (num >= 0 && num < branches.size()) {
                a.setWorkBranch(&branches[num]);
                cout << "\nFound branch!\n";
            }
        }
    }
}

void InitBank(Bank& bank) {
    fstream file("bank.bin", ios::binary);
    file.read((char*)&bank, sizeof(Bank));
    file.close();
}
void SaveBank(Bank& bank) {
    fstream file("bank.bin", ios::binary | ios::out);
    file.write((char*)&bank, sizeof(Bank));
    file.close();
}
//|----------------------Bank & Branch Test Menu----------------------|

//|----------------------Functions----------------------|

int main() {
    // vector<Position> position;  // removed, position history managed by WorkerPosada
    srand((unsigned)time(NULL));
    vector<Person> people;
    vector<Worker> worker;
    vector<Client> client;
    Bank bank;
    vector<BankBranch> branch;
    vector<WorkerPosada> wposada;
    vector<Account> accounts;
    const char* account_filename = "accounts.bin";
    const char* ids_filename = "ids.bin";
    const char* workerposada_filename = "workerposada.bin";
    const char* filename = "person.bin";
    const char* worker_filename = "worker.bin";
    const char* bank_filename = "bank.bin";
    const char* branch_filename = "branch.bin";
    const char* client_filename = "client.bin";
    const char* transaction_filename = "transaction.bin";
    const char* credit_filename = "credit.bin";
    const char* mainID = "1424672248";
    int choice=0;

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    //|-------------AUTOSTART--------------|
    readFromFile(filename, people);
    readFromFile(worker_filename, worker);
    readFromFile(branch_filename, branch);
    readFromFile(client_filename, client);
    readFromFile(workerposada_filename, wposada);
    readFromFile(transaction_filename, transactions);
    readFromFile(credit_filename, credits);
    readFromFile(account_filename, accounts);
    InitBank(bank);
    for (auto& a : credits)a.recalcCredit();
    bank.setName("Bank of Ukraine");
    bank.setBankID(1);
    bank.setMoney(1000000);
    if (branch.empty()) { BankBranch mainBranch(1, 1, "Instutytska 11/3");branch.push_back(mainBranch); }
    if (people.empty()){
        Person TestAdmin("Admin", "Admin", "Admin", 777);
        Person TestWorker("worker", "worker", "worker", 666);
        Person TestUser("user", "user", "user", 555);people.push_back(TestAdmin);people.push_back(TestWorker);people.push_back(TestUser);
        if (worker.empty()){
            int wid = TestWorker.getPersonID();
            Worker TestBorker(TestWorker, wid);TestBorker.setWorkBankID(1);TestBorker.setWorkBranchID(1);
            worker.push_back(TestBorker);
        }
    }
    if (accounts.empty()) {
        Account admin("admin", "admin", "admin", 777);
        Account workerAcc("worker", "worker", "worker", 666);
        Account user("user", "user", "user", 555);
        accounts.push_back(admin);accounts.push_back(workerAcc);accounts.push_back(user);
    }
    linkBank(branch,bank);
    linkWorkers(branch, worker);
    LoginMenu(people, worker, client, bank, branch, wposada,accounts);
    Position unemployedPos;unemployedPos.setSalary(0);unemployedPos.setPosition("unemployed");
//|-------------AUTOSTART--------------|  
    // Save data to files
    writeToFile(account_filename, accounts);
    writeToFile(filename, people);
    writeToFile(worker_filename, worker);
    writeToFile(branch_filename, branch);
    writeToFile(client_filename, client);
    writeToFile(workerposada_filename, wposada);
    writeToFile(transaction_filename, transactions);
    writeToFile(credit_filename, credits);
    SaveBank(bank);
    cout << "Data saved to files.\n";
    return 0;
}