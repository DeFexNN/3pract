#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <ctime>
#include <Windows.h>


////////////////////// Logger class for logging messages to a file with timestamps///////////////////////
class Logger {
    std::ofstream ofs;
public:
    Logger(const char* filename) { ofs.open(filename, std::ios::app); }
    template<typename T>
    Logger& operator<<(const T& msg) {
        time_t now = time(nullptr);
        tm* lt = localtime(&now);
        char buf[64];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt);
        ofs << "[" << buf << "] " << msg;
        ofs.flush();
        return *this;
    }

    // overload for stream manipulators like std::endl
    Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
        manip(ofs);
        return *this;
    }

    // allow manual flush
    void flush() {
        ofs.flush();
    }
};
////////////////////// Logger class for logging messages to a file with timestamps///////////////////////
static Logger logger("log.txt");

//клас WorkerPosada наслідує 2 класи робітник і посади і там буде дата прийняття і дата звільнення 
using namespace std;

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} dtime;

typedef struct {
    char login[20];
    char password[20];
    char role[20];
}account;
account accounts[3]{
    {"admin", "admin","admin"},
    {"worker", "worker","worker"},
    {"user", "user","user"}
};
//|-----------------------------CLASSES----------------------------|
const int STR_SIZE = 32;
class BankBranch;
class Client;
class Bank;
int AdminBranchesMenu(vector<BankBranch>& branches, Bank& bank);
char curUser[20];
dtime tmToCtime(const tm& timeinfo);
class Credit {
    int amount;
    dtime take_date;
    dtime return_date;
    int percent;
public:
    Credit(int amount, dtime take_date, dtime return_date, int percent) : amount(amount), take_date(take_date), return_date(return_date), percent(percent) {}
    void showInfo() {
        cout << "Credit amount: " << amount << "\nTake date: " << take_date.year << "-" << take_date.month << "-" << take_date.day
            << " " << take_date.hour << ":" << take_date.minute << ":" << take_date.second
            << "\nReturn date: " << return_date.year << "-" << return_date.month << "-" << return_date.day
            << " " << return_date.hour << ":" << return_date.minute << ":" << return_date.second
            << "\nPercent: " << percent << "%" << endl;
    }
};
static vector<Credit> credits;
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
    int ID=0;
    int WorkBranchID=0;
    int WorkBankID = 0;
    // positions storage removed; position history managed by WorkerPosada
    BankBranch* workBranch = nullptr;
public:
    Worker() : ID(0) {  }
    Worker(const Person& p, int ID) : Person(p), ID(ID) {}
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
    void setWorkBranch(BankBranch* branch) { workBranch = branch; } 
};
class WorkerPosada : public Worker, public Position {
    dtime employDate{};
    dtime fireDate{};
public:
    WorkerPosada() = default;
    WorkerPosada(const Worker& worker, const Position& position, const dtime& employDate)
        : Worker(worker), Position(position), employDate(employDate) {}
    void setFireDate(const dtime& fd) { fireDate = fd; }
    void showInfo() {
        cout << "\n|--- Position History ---|\n"
             << "Worker ID: " << getID() << "\nPosition: " << getPosition()
             << "\nSalary: " << getSalary()
             << "\nEmploy date: " << employDate.year << "-" << employDate.month << "-" << employDate.day
             << "\nFire date: ";
        if (fireDate.year) cout << fireDate.year << "-" << fireDate.month << "-" << fireDate.day;
        else cout << "N/A";
        cout << endl;
    }
};
class Client : public Person {
    int balance=0;
    int clientID=0;
    int bankID=0;
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

    int WithdrawMoney(int num, Client& client, vector<BankBranch>& branches) { if (client.getClientBalance() <= num)cout << "You dont have enough money";else { if (num < getBranchMoney()) { client.increaseClientBalance(num);setBranchMoney(getBranchMoney() - num); // record transaction
                dtime dt = getCurrentTime();
                char from_str[20], to_str[20];
                snprintf(from_str, 20, "branch%d", branchNUM);
                snprintf(to_str, 20, "client%d", client.getClientID());
                transactions.emplace_back(num, dt, "withdraw", from_str, to_str, client.getClientID()); } else { if (bank_ptr->getAllBankMoney(branches) > num) { if (bank_ptr->WithdrawMoney(num, branches, client) == 1)return 1; // record transaction via bank pathway
                        dtime dt = getCurrentTime();
                        char from_str[20], to_str[20];
                        snprintf(from_str, 20, "bank%d", bankID);
                        snprintf(to_str, 20, "client%d", client.getClientID());
                        transactions.emplace_back(num, dt, "withdraw", from_str, to_str, client.getClientID()); } } } return 0; }
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
        credits.emplace_back(amount, take, ret, percent);
        // record transaction
        char from_str[20], to_str[20];
        snprintf(from_str,20,"branch%d",branchNUM);
        snprintf(to_str,20,"client%d",client.getClientID());
        transactions.emplace_back(amount, take, "credit", from_str, to_str, client.getClientID());
    }

};
int Bank::WithdrawMoney(int num, vector<BankBranch>& branches, Client& client) { recalcAllMoneyFromBranches(branches);if (money >= num) { money -= num; client.increaseClientBalance(num); } else if (getAllBankMoney(branches) > num) { WithdrawMoneyFromBranches(num, branches);money -= num;client.increaseClientBalance(num); } else { cout << "Cannot withdraw: insufficient funds.\n";return 1; } return 0; }
int Bank::transferMoneyToBranch(BankBranch& branch, int num) { if (num <= money) { money -= num;branch.increaseBranchMoney(num);return 0; } else return 1; }
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
bool isBranchNumUnique(int num, int BankID, vector<BankBranch>& branches) {
    for (auto& a : branches)if (num == a.getBranchNum() && BankID == a.getBranchID())return false;
    return true;
}
//функція для знаходження актуального вказівника на головний банк по айді
void linkBank(vector<BankBranch>& branches, Bank& bank) {
    for (auto& branch : branches) {
        branch.setBankPointer(&bank);
        cout << "\nFound!\n"; // видаліть або закоментуйте
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
            Person np; np.inputFromConsole();
            person.push_back(np);
        } else if (ch == 3) {
            cout << "Index to edit: "; int i; cin >> i;
            if (i >= 1 && i <= (int)person.size()) person[i-1].inputFromConsole();
        } else if (ch == 4) {
            cout << "Index to remove: "; int i; cin >> i;
            if (i >= 1 && i <= (int)person.size()) person.erase(person.begin() + i - 1);
        } else if (ch == 5) {
            break;
        } else cout << "Wrong choice!\n";
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
             << "4) Back\n"
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
            cout << "Enter worker ID: "; int id; cin >> id;
            cout << "Enter position name: "; string posName; cin >> posName;
            cout << "Enter salary: "; int sal; cin >> sal;
            // enter hire date
            cout << "Enter hire date:\n";
            dtime hireDate = inputCtime();
            Position pos; pos.setPosition(posName.c_str()); pos.setSalary(sal);
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
            cout << "Index to remove: "; int i; cin >> i;
            if (i >= 1 && i <= (int)workers.size()) {
                workers.erase(workers.begin() + i - 1);
                // also remove related position histories
                wposada.erase(remove_if(wposada.begin(), wposada.end(), [&](WorkerPosada& wp){ return wp.getID() == workers[i-1].getID(); }), wposada.end());
            }
        } else if (ch == 4) {
            break;
        } else cout << "Wrong choice!\n";
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
    vector<WorkerPosada>& wposada
) {
    logger << "User entered as admin!";
    while (true) {
        cout << "\n=== ADMIN MENU ===\n"
             << "1) people menu\n"
             << "2) workers menu\n"
             << "3) clients menu\n"
             << "4) banks menu\n"
             << "5) branches menu\n"
             << "6) Exit menu\n"
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
        } else {
            cout << "Wrong choice!\n";
        }
    }
    return 0;
}

int WorkerMenu(vector<Client>& clients, Bank &bank, vector<BankBranch>& branches, vector<Worker>& workers);
// User menu with clients, bank, branches and workers
int UserMenu(vector<Client>& clients, Bank &bank, vector<BankBranch>& branches, vector<Worker>& workers);
int LoginMenu(
    vector<Person>&person,
    vector<Worker>&workers,
    vector<Client>&clients,
    Bank &bank,
    vector<BankBranch>&branches,
    vector<WorkerPosada>&wposada
) {
    logger << "Login menu started\n"; logger.flush();
        while (true) {//вхід в аккаунт за роллю
            bool found = false;
            char login[20], password[20];
            cout << "Now enter login: "; cin >> login;
            cout << "Now enter password: "; cin >> password;
            for (auto& a : accounts) {
                if (strcmp(a.login, login) == 0 && strcmp(a.password, password) == 0) {
                    found = true;
                    if (a.role == string("admin")) {
                        AdminMenu(person, workers, clients, bank, branches, wposada);
                    } else if (a.role == string("worker")) {
                        WorkerMenu(clients, bank, branches, workers);
                    } else if (a.role == string("user")) {
                        UserMenu(clients, bank, branches, workers);
                    }
                    break;
                }
            }
            if (found) break;
            else { cout << "Enter correct login & password!\n"; }
        }
        return 0;
}





int WorkerMenu(vector<Client>& clients, Bank& bank, vector<BankBranch>& branches, vector<Worker>& workers) {
    logger << "User entered as worker!";
    return 0;
}



int UserMenu(vector<Client>& clients,
                 Bank& bank,
                 vector<BankBranch>& branches,
                 vector<Worker>& workers) {
    //|----------------------Identity----------------------|
    cout << "Who are you?\n";
    for (int idx = 0; idx < clients.size(); ++idx) {
        cout << idx+1 << ") " << clients[idx] << endl;
    }
    int sel; cin >> sel;
    if (sel < 1 || sel > clients.size()) { cout << "Invalid choice!\n"; return 1; }
    Client& curCl = clients[sel-1];
    logger << "Logged as user: " << curCl.getName() << " " << curCl.getSurname() << std::endl;
    cout << "You are logged as: " << curCl << std::endl;

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
        if (w.getWorkBankID() == br.getBankID() && w.getWorkBranchID() == br.getBranchNum())
            bw.push_back(&w);
    if (bw.empty()) { cout << "No worker available at this branch.\n"; return 1; }
    cout << "Select worker:\n";
    for (int i = 0; i < bw.size(); ++i)
        cout << i+1 << ") " << bw[i]->getName() << " " << bw[i]->getSurname() << "\n";
    int wi; cin >> wi;
    if (wi < 1 || wi > bw.size()) { cout << "Invalid worker\n"; return 1; }
    Worker* serv = bw[wi-1];

    //|----------------------User Operations---------------------|
    while (true) {
        cout << "\n=== USER MENU ===\n"
             << "1) Show balance\n"
             << "2) Withdraw money\n"
             << "3) Deposit money\n"
             << "4) Show transaction history\n"
             << "5) Take credit\n"
             << "6) Exit\n"
             << "Enter: "; int choice; cin >> choice;
        if (choice == 1) {
            cout << "Your balance: " << curCl.getClientBalance() << "\n";
        } else if (choice == 2) {
            cout << "Amount to withdraw: "; int amt; cin >> amt;
            serv->withdrawMoneyFromBranch(curCl, amt, branches);
        } else if (choice == 3) {
            cout << "Amount to deposit: "; int amt; cin >> amt;
            serv->putMoneyToBranch(curCl, amt);
        } else if (choice == 4) {
            showClientTransactions(curCl.getClientID());
        } else if (choice == 5) {
            cout << "Credit amount: "; int amt; cin >> amt;
            cout << "Percent rate (%): "; int pct; cin >> pct;
            cout << "Term days: "; int days; cin >> days;
            serv->giveCreditToBranch(curCl, amt, pct, days);
        } else if (choice == 6) {
            break;
        } else {
            cout << "Wrong choice!\n";
        }
    }
    //|----------------------End User Operations---------------------|
    return 0;
}


















void linkWorkers(vector<BankBranch> branches,vector<Worker> workers) {
    for (auto& a : workers) {
        for (auto& b : branches) {
            if (a.getWorkBankID() == b.getBankID() && a.getWorkBranchID() == b.getBranchNum()) {
                a.setWorkBranch(&b);
                cout << "\nFound branch!\n"; // видаліть або закоментуйте
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
    vector<Person> people;
    vector<Worker> worker;
    vector<Client> client;
    Bank bank;
    vector<BankBranch> branch;
    vector<WorkerPosada> wposada;
    const char* workerposada_filename = "workerposada.bin";
    const char* filename = "person.bin";
    const char* worker_filename = "worker.bin";
    const char* bank_filename = "bank.bin";
    const char* branch_filename = "branch.bin";
    const char* client_filename = "client.bin";
    const char* transaction_filename = "transaction.bin";
    int choice;

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
//|-------------AUTOSTART--------------|
    readFromFile(filename, people);
    readFromFile(worker_filename, worker);for (auto& w : worker) w.findWorkBranch(branch);
    readFromFile(branch_filename, branch);
    readFromFile(client_filename, client);
    readFromFile(workerposada_filename, wposada);
    readFromFile(transaction_filename, transactions);
    InitBank(bank);
    bank.setName("Bank of Ukraine");
    bank.setBankID(1);
    bank.setMoney(1000000);
    linkBank(branch,bank);
    linkWorkers(branch, worker);
    LoginMenu(people, worker, client, bank, branch, wposada);
    Position unemployedPos;unemployedPos.setSalary(0);unemployedPos.setPosition("unemployed");
//|-------------AUTOSTART--------------|  
    /*
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
            int count = 1, chose; cout << "Chose person to do worker:\n";
            for (auto& a : people) {
                cout << count << ")" << "Name: " << a.getName() << "\nSurname: " << a.getSurname() << "\nLastname: " << a.getLastname() << endl; count++;
            } cout << count << ")Create new person\nchoice: ";
            cin >> chose;
            if (chose == count) {
                Person newPerson;
                newPerson.inputFromConsole();
                people.push_back(newPerson);
                cout << "Person added.\n";
                char posName[STR_SIZE]; int salary;
                cout << "Enter position name: "; cin >> posName;
                cout << "Enter salary: "; cin >> salary;
                Position pos; pos.setPosition(posName); pos.setSalary(salary);
                int tempid;
                while (true) {
                    cout << "Enter ID: "; cin >> tempid; if (isIDUnique(tempid, worker)) break; cout << "Enter another id thats ID corrupted: ";
                }
                Worker tempWorker(people.back(), tempid, pos);
                worker.push_back(tempWorker);
            }
            else {
                char posName[STR_SIZE]; int salary;
                cout << "Enter position name: "; cin >> posName;
                cout << "Enter salary: "; cin >> salary;
                Position pos; pos.setPosition(posName); pos.setSalary(salary);

                int bankChoice=0;
                if (bank.empty()) {
                    cout<<"No banks available. Create one first.\n";
                } else {
                    cout<<"Select bank:\n";
                    for(int i=0;i<bank.size();++i)
                        cout<<i+1<<") "<<bank[i].getBankID()<<" - "<<endl;
                    cin>>bankChoice;
                }
                int branchChoice=0;
                vector<BankBranch*> filtered;
                for(auto& br: branch)
                    if(bankChoice>=1 && bankChoice<=bank.size() && br.getBankID()==bank[bankChoice-1].getBankID())
                        filtered.push_back(&br);
                if(filtered.empty()) {
                    cout<<"No branches for that bank. Create one first.\n";
                } else {
                    cout<<"Select branch:\n";
                    for(int i=0;i<filtered.size();++i)
                        cout<<i+1<<") "<<filtered[i]->getBranchNum()<<endl;
                    cin>>branchChoice;
                }

                int tempid;
                while (true) {
                    cout << "Enter ID: "; cin >> tempid; if (isIDUnique(tempid, worker)) break; cout << "Enter another id thats ID corrupted: ";
                }

                Worker tempWorker(people[chose - 1], tempid, pos);
                if(bankChoice>0) tempWorker.setWorkBankID(bank[bankChoice-1].getBankID());
                if(branchChoice>0) tempWorker.setWorkBranchID(filtered[branchChoice-1]->getBranchNum());
                tempWorker.findWorkBranch(branch);
                worker.push_back(tempWorker);
            }
        }
        else {
            cout << "Invalid option!\n";
        }
    }
        */
    // Save data to files
    writeToFile(filename, people);
    writeToFile(worker_filename, worker);
    writeToFile(branch_filename, branch);
    writeToFile(client_filename, client);
    writeToFile(workerposada_filename, wposada);
    writeToFile(transaction_filename, transactions);
    SaveBank(bank);
    cout << "Data saved to files.\n";
    return 0;
}

















//|----------------------Bank & Branch Test Menu----------------------|
/*
void BankAndBranchTestMenu() {
    vector<Bank> banks;
    vector<BankBranch> branches;
    int choice;
    while (true) {
        cout << "\n==== BANK & BRANCH TEST MENU ====\n";
        cout << "1. Create Bank\n";
        cout << "2. Create Branch\n";
        cout << "3. Link Branches to Banks\n";
        cout << "4. Show Banks\n";
        cout << "5. Show Branches\n";
        cout << "6. Test Bank Functions\n";
        cout << "7. Test Branch Functions\n";
        cout << "8. Exit to main\n";
        cout << "Choose: ";
        cin >> choice;
        if (choice == 1) {
            int money;
            char name[20];
            cout << "Enter bank name: "; cin >> name;
            cout << "Enter bank money: "; cin >> money;
            Bank b(money, name);
            b.setBankID(banks.size() + 1);
            banks.push_back(b);
            cout << "Bank created with ID: " << b.getBankID() << endl;
        }
        else if (choice == 2) {
            if (banks.empty()) { cout << "Create a bank first!\n"; continue; }
            int bankid;
            cout << "Available banks:\n";
            for (int i = 0; i < banks.size(); ++i) {
                cout << i + 1 << ") "; banks[i].showInfo();
            }
            cout << "Enter bank number to link branch: "; cin >> bankid;
            if (bankid < 1 || bankid > banks.size()) { cout << "Invalid bank!\n"; continue; }
            cout << "Enter branch address: ";char addr[50];cin.ignore();cin.getline(addr, 50);int branchNum;
            while (true) {
                cout << "Enter branch number: ";
                cin >> branchNum;
                if (isBranchNumUnique(branchNum, bankid, branches)) {
                    break;
                }
                else {
                    cout << "Branch number already exists in this bank!\n";
                }
            }
            BankBranch br(banks[bankid - 1].getBankID(), branchNum, addr);
            br.setBankPointer(&banks[bankid - 1]);
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

        }
        else if (choice == 3) {
            if (banks.empty() || branches.empty()) { cout << "Create banks and branches first!\n"; continue; }
            linkBank(branches, banks);
        }
        else if (choice == 4) {
            if (banks.empty()) cout << "No banks.\n";
            else for (auto& b : banks) { b.showInfo(); b.displayActualBranches(branches); }
        }
        else if (choice == 5) {
            if (branches.empty()) cout << "No branches.\n";
            else {
                int idx = 1;
                for (auto& br : branches) {
                    cout << idx++ << ") Branch for BankID: " << br.getBranchID()
                         << ", Money: " << br.getBranchMoney() << endl;
                }
            }
        }
        else if (choice == 6) {
            if (banks.empty()) { cout << "No banks.\n"; continue; }
            int bidx;
            cout << "Select bank to test: ";
            for (int i = 0; i < banks.size(); ++i) {
                cout << i + 1 << ") "; banks[i].showInfo();
            }
            cin >> bidx;
            if (bidx < 1 || bidx > banks.size()) { cout << "Invalid!\n"; continue; }
            Bank& b = banks[bidx - 1];
            cout << "Bank total money (with branches): " << b.getAllBankMoney(branches) << endl;
            cout << "Branches money: " << b.getBranchesMoney(branches) << endl;
            cout << "Branches count: " << b.getBranchesCount(branches) << endl;
            cout << "Recalculating all money from branches...\n";
            b.recalcAllMoneyFromBranches(branches);
            cout << "After recalculation:\n";
            for (auto& br : branches) {
                if (br.getBranchID() == b.getBankID())
                    cout << "Branch money: " << br.getBranchMoney() << endl;
            }
        }
        else if (choice == 7) {
            if (branches.empty()) { cout << "No branches.\n"; continue; }
            int bridx;
            cout << "Select branch to test: \n";
            for (int i = 0; i < branches.size(); ++i) {
                cout << i + 1 << ") Branch for BankID: " << branches[i].getBranchID()
                     << ", Money: " << branches[i].getBranchMoney() << endl;
            }
            cin >> bridx;
            if (bridx < 1 || bridx > branches.size()) { cout << "Invalid!\n"; continue; }
            BankBranch& br = branches[bridx - 1];
            cout << "Branch money: " << br.getBranchMoney() << endl;
            cout << "Decrease branch money by: "; int dec; cin >> dec;
            br.decreaseBranchMoney(dec);
            cout << "Now branch money: " << br.getBranchMoney() << endl;
        }
        else if (choice == 8) {
            break;
        }else if(choice == 9) {
            int num; cout << "Enter 1 of you want to write to file, else read==2: "; cin >> num;
            const char* bank_filename = "bank.bin";
            const char* branch_filename ="bank_branch.bin";
            while (true) {
                if (num == 1) {
                    readFromFile(branch_filename, branches);
                    break;
                }
                else cout << "Wrong choice!\n";
            }
        }
        else {
            cout << "Invalid option!\n";
        }
    }
}
*/