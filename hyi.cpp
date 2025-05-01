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
    Person() { name[0]=surname[0]=lastname[0]=0; }

    Person(const char* n, const char* s, const char* l) {
        strncpy(name, n, STR_SIZE); name[STR_SIZE-1]=0;
        strncpy(surname, s, STR_SIZE); surname[STR_SIZE-1]=0;
        strncpy(lastname, l, STR_SIZE); lastname[STR_SIZE-1]=0;
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
    Worker() : ID(0) { position[0]=0; }
    Worker(const Person& p, const char* pos, int ID) : Person(p), ID(ID) {
        strncpy(position, pos, STR_SIZE); position[STR_SIZE-1]=0;
    }
    void ShowInfo() {
        cout << "Name: " << getName() << "\nSurname: " << getSurname() << "\nLastname: " << getLastname() << "\nID: " << ID << "\nPosition: " << position << endl;
    }
    int getID() const { return ID; }
    string getPosition() const { return position; }
};
//|-----------------------------CLASSES----------------------------|


//|----------------------Functions----------------------|

template <typename T>
void writeToFile(const char* filename, const vector<T>& data) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Can't open file for writing.\n";
        return;
    }
    for (const auto& obj : data) {
        file.write((char*)(&obj), sizeof(T));
    }
    file.close();
}

template <typename T>
void readFromFile(const char* filename, vector<T>& data) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Can't open file for reading.\n";
        return;
    }
    data.clear();
    T obj;
    while (file.read((char*)(&obj), sizeof(T))) {
        data.push_back(obj);
    }
    file.close();
}

bool isIDUnique(int id, vector<Worker>& wk) {
    for (auto& w : wk) if (w.getID() == id) return false;
    return true;
}

void showMenu() {
    cout << "\n==== MENU ====\n";
    cout << "1. Load people from file\n";
    cout << "2. Add new person\n";
    cout << "3. Show all people\n";
    cout << "4. Save to file\n";
    cout << "5. Exit\n";
    cout << "6. Create worker from person\n";
    cout << "Choose: ";
}

//|----------------------Functions----------------------|

int main() {
    vector<Person> people;
    vector<Worker> worker;
    const char* filename = "person.bin";
    const char* worker_filename = "worker.bin";
    int choice;

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
        else {
            cout << "Invalid option!\n";
        }
    }
    return 0;
}
