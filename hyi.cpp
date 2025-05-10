#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <ctime>



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
char curUser[20];
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
    vector<Position> positions;
    BankBranch* workBranch = nullptr;
public:
    Worker() : ID(0) {  }
    Worker(Person& p, int ID, Position& position) : Person(p), ID(ID) { positions.push_back(position); }
    void ShowInfo() { 
        cout << "\n|-------------------|\nName: " << getName() << "\nSurname: " << getSurname() << "\nLastname: " << getLastname() << "\nID: " << ID << "\nPositions:\n"; 
        for (int i = 0; i < positions.size(); ++i) { 
            cout << i+1 << ") " << positions[i].getPosition() << ", salary: " << positions[i].getSalary(); 
            if(i == positions.size()-1) cout << "  <-- active"; 
            cout << endl; 
        } 
    }
    bool findWorkBranch(vector<BankBranch>& branches);
    int getID() { return ID; }
    void getPositionInfo() { if (!positions.empty())positions.back().showInfo();else cout << "This worker dont have positions!\n"; }
    void addPosition(Position& position) { positions.push_back(position); }
    void removePosition() { if(!positions.empty()) positions.pop_back(); }
    void setPositionSalary(int idx, int salary) { if(idx>=0 && idx<(int)positions.size()) positions[idx].setSalary(salary); }
    int getPositionCount() { return (int)positions.size(); }
    void setWorkBankID(int id) { WorkBankID = id; }
    void setWorkBranchID(int id) { WorkBranchID = id; }

    //menu where worker do his work
    void withdrawMoneyFromBranch(Client& client, int num, vector<BankBranch>& branches);
    void putMoneyToBranch(Client& client, int num);
};
class WorkerPosada : public Worker , public Position {
    dtime employdate{};
    dtime firedate{};
public:
    WorkerPosada(Worker& worker, Position& position) :Worker(worker), Position(position) {}
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

    int WithdrawMoney(int num, Client& client, vector<BankBranch>& branches) { if (client.getClientBalance() < num)cout << "You dont have enough money";else { if (num < getBranchMoney()) { client.increaseClientBalance(num);setBranchMoney(getBranchMoney() - num); } else { if (bank_ptr->getAllBankMoney(branches) > num) { if (bank_ptr->WithdrawMoney(num, branches, client) == 1)return 1; } } } return 0; }
    //void PutMoney(int num, Client& cl, vector<BankBranch>& branches) { if (cl.getClientBalance() < num)cout << "You have no money!\n";else { if (num > this->money && num <= bank_ptr->getAllBankMoney(branches)); } }
    void PutMoney(int num, Client& client) { client.increaseClientBalance(num);increaseBranchMoney(num); }

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
//сама перша менюшка
void showMenu() {
    cout << "\n==== MENU ====\n";
    cout << "1. Load people from file\n";
    cout << "2. Add new person\n";
    cout << "3. Show all people\n";
    cout << "4. Save to file\n";
    cout << "5. Exit\n";
    cout << "6. Create worker from person\n";
    cout << "7. Bank & Branch Test\n";
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
//|----------------------Bank & Branch Test Menu----------------------|
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
                    writeToFile(bank_filename, banks);
                    writeToFile(branch_filename, branches);
                    cout << "Succesfully writed into file!\n";
                    break;
                }
                else if (num == 2) {
                    readFromFile(bank_filename, banks);
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























int AdminMenu();
int WorkerMenu();
int UserMenu();
int LoginMenu(
    vector<Person>&person,
    vector<Worker>&workers,
    vector<Client>&clients,
    vector<Bank>&banks,
    vector<BankBranch>&branches,
    vector<Position>&positions,
    vector<WorkerPosada>&wposada
) {
    logger << "Login menu started\n"; logger.flush();
    while (true) {
        while (true) {//вибір ролі за якою логінитися
            cout << "Enter your role!\n1)admin\n2)worker\n3)user\nEnter: ";int choice = 0;char role[20];cin >> choice;
            if (choice == 1) { strcpy(role, "admin");break; }
            else if (choice == 2) { strcpy(role, "worker");break; }
            else if (choice == 3) { strcpy(role, "user");break; }
            else cout << "Wrong choice!";
        }
        while (true) {//вхід в аккаунт за роллю
            bool found=false;
            cout << "Now enter login";char login[20];cin.getline(login, 20);
            cout << "Now enter password";char password[20];cin.getline(password, 20);
            for (auto& a : accounts) {
                if (a.role == "admin" && strcmp(a.login, login) == 0 && strcmp(a.password, password)) { AdminMenu(person,workers,clients,banks,branches,positions,wposada);break; }
                else if (a.role == "worker" && strcmp(a.login, login) == 0 && strcmp(a.password, password)){ WorkerMenu();break; }
                else if (a.role == "user" && strcmp(a.login, login) == 0 && strcmp(a.password, password)) { UserMenu();break; }
            }if (found)break;else { cout << "Enter correct login&pass!"; }
        }
    }
}




int AdminMenu(
    vector<Person>& person,
    vector<Worker>& workers,
    vector<Client>& clients,
    vector<Bank>& banks,
    vector<BankBranch>& branches,
    vector<Position>& positions,
    vector<WorkerPosada>& wposada
) {
    logger << "User entered as admin!";
    return 0;
}




int WorkerMenu() { return 0; }




int UserMenu(vector<Person>&persons) {
    //|----------------------Identity----------------------|
    Person curPerson;
    cout << "Who are you?";int i = 0;for (auto& a : persons) { cout<<i+1<<')' << a << endl; }
    curPerson = persons[i];logger << "Logged as default user: " << curPerson;
    cout << "You are logged as: " << curPerson << endl;
    //|----------------------Identity----------------------|
    while (true) {
        cout << "1)Bank operations\n2)Exit\nEnter: ";int choice;cin >> choice;
    }
    return 0;
}

























//|----------------------Bank & Branch Test Menu----------------------|

//|----------------------Functions----------------------|

int main() {
    vector<Position> position;
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
    LoginMenu();
    linkBank(branch,bank);
    Position unemployedPos;unemployedPos.setSalary(0);unemployedPos.setPosition("unemployed");
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
        else if (choice == 7) {
            BankAndBranchTestMenu();
        }
        else {
            cout << "Invalid option!\n";
        }
    }
    return 0;
}