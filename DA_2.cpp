/* Project Title: Mini Banking System-“Personal Savings Account” 
Student Name: Sneha Sahu
 Register No: 25BCE5319
*/ 
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <limits>
#include <sstream>

using namespace std;

class Transaction { 
private:
    int transNo;
    string type;
    double amount;
    string dateTime;

public:
    Transaction(int tNo, const string& tType, double amt) {
        transNo = tNo;
        type = tType;
        amount = amt;

        time_t now = time(0);
        char* dt = ctime(&now);
        dateTime = string(dt);
        if(!dateTime.empty() && dateTime.back() == '\n') dateTime.pop_back();
    }

    string getTransactionInfo() const {
        stringstream ss;
        ss << transNo << ". " << type << " of Rs. "
           << fixed << setprecision(2) << amount
           << " on " << dateTime;
        return ss.str();
    }

    string serialize() {
        return to_string(transNo) + "|" + type + "|" +
               to_string(amount) + "|" + dateTime;
    }
};

class Account {
private:
    int accNo;
    string name;
    double balance;
    int pin;   
    vector<Transaction> transactions;

public:
    Account(int acc, const string& nm, int p, double bal = 0.0) {
        accNo = acc;
        name = nm;
        pin = p;
        balance = bal;
    }

    int getAccNo() const { return accNo; }
    string getName() const { return name; }
    double getBalance() const { return balance; }

    bool verifyPIN(int p) const {
        return p == pin;
    }

    void deposit(double amount) {
        balance += amount;
        int tNo = transactions.size() + 1;
        transactions.push_back(Transaction(tNo, "Deposit", amount));
        cout << "Deposit successful. New Balance: Rs. "
             << fixed << setprecision(2) << balance << endl;
    }

    void withdraw(double amount) {
        if(amount > balance) {
            cout << "Insufficient balance.\n";
            return;
        }
        balance -= amount;
        int tNo = transactions.size() + 1;
        transactions.push_back(Transaction(tNo, "Withdraw", amount));
        cout << "Withdrawal successful. New Balance: Rs. "
             << fixed << setprecision(2) << balance << endl;
    }

    void showBalance() const {
        cout << "Account No: " << accNo
             << " | Name: " << name
             << " | Balance: Rs. "
             << fixed << setprecision(2) << balance << endl;
    }

    void showLastTransactions(int n = 5) const {
        cout << "Last " << n << " transactions:\n";
        int start = max(0, int(transactions.size()) - n);
        for(size_t i = start; i < transactions.size(); i++) {
            cout << transactions[i].getTransactionInfo() << endl;
        }
    }

    string serialize() {
        return to_string(accNo) + "|" + name + "|" +
               to_string(pin) + "|" + to_string(balance);
    }

    static Account deserialize(const string& line) {
        stringstream ss(line);
        string token;
        vector<string> parts;

        while(getline(ss, token, '|')) {
            parts.push_back(token);
        }
        return Account(stoi(parts[0]), parts[1],
                       stoi(parts[2]), stod(parts[3]));
    }
    vector<Transaction>& getTransactions() { return transactions; }
};

class BankSystem {
private:
    vector<Account> accounts;
    const string accountsFile = "accounts.txt";
    const string transactionsFile = "transactions.txt";

    Account* findAccount(int accNo) {
        for(auto &acc : accounts) {
            if(acc.getAccNo() == accNo)
                return &acc;
        }
        return nullptr;
    }

    // PIN Authentication with 3 attempts
    bool authenticate(Account* acc) {
        int p;
        for(int i = 0; i < 3; i++) {
            cout << "Enter PIN: ";
            cin >> p;

            if(acc->verifyPIN(p))
                return true;

            cout << "Incorrect PIN. Attempts left: " << 2 - i << endl;
        }
        cout << "Access denied!\n";
        return false;
    }

public:
    BankSystem() {
        loadAccounts();
        loadTransactions();
    }

    ~BankSystem() {
        saveAccounts();
        saveTransactions();
    }

    void createAccount() {
        int accNo, pin;
        string name;
        double initBalance;
        cout << "Enter Account Number: ";
        cin >> accNo;
        if(findAccount(accNo)) {
            cout << "Account already exists.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Name: ";
        getline(cin, name);
        cout << "Set 4-digit PIN: ";
        cin >> pin;
        if(pin < 1000 || pin > 9999) {
            cout << "Invalid PIN! Must be 4 digits.\n";
            return;
        }
        cout << "Enter Initial Balance: ";
        cin >> initBalance;
        if(initBalance < 0) initBalance = 0;

        accounts.push_back(Account(accNo, name, pin, initBalance));
        cout << "Account created successfully!\n";
    }
    void depositToAccount() {
        int accNo;
        double amount;
        cout << "Enter Account Number: ";
        cin >> accNo;

        Account* acc = findAccount(accNo);
        if(!acc) {
            cout << "Account not found!\n";
            return;
        }

        if(!authenticate(acc)) return;

        cout << "Enter Amount: ";
        cin >> amount;
        if(amount <= 0) {
            cout << "Invalid amount.\n";
            return;
        }
        acc->deposit(amount);
    }

    void withdrawFromAccount() {
        int accNo;
        double amount;

        cout << "Enter Account Number: ";
        cin >> accNo;
        Account* acc = findAccount(accNo);
        if(!acc) {
            cout << "Account not found!\n";
            return;
        }

        if(!authenticate(acc)) return;
        cout << "Enter Amount: ";
        cin >> amount;
        if(amount <= 0) {
            cout << "Invalid amount.\n";
            return;
        }
        acc->withdraw(amount);
    }

    void checkBalance() {
        int accNo;
        cout << "Enter Account Number: ";
        cin >> accNo;

        Account* acc = findAccount(accNo);
        if(!acc) {
            cout << "Account not found!\n";
            return;
        }

        if(!authenticate(acc)) return;
        acc->showBalance();
    }

    void showTransactions() {
        int accNo;
        cout << "Enter Account Number: ";
        cin >> accNo;
        Account* acc = findAccount(accNo);
        if(!acc) {
            cout << "Account not found!\n";
            return;
        }

        if(!authenticate(acc)) return;
        acc->showLastTransactions();
    }

    void totalMoney() {
        string adminPass;
        cout << "Enter Admin Password: ";
        cin >> adminPass;
        if(adminPass != "admin123") {
            cout << "Unauthorized access!\n";
            return;
        }
        double total = 0;
        for(auto &acc : accounts)
            total += acc.getBalance();
        cout << "Total Bank Balance: Rs. "
             << fixed << setprecision(2) << total << endl;
    }
    void saveAccounts() {
        ofstream fout(accountsFile);
        for(auto &acc : accounts)
            fout << acc.serialize() << endl;
    }
    void loadAccounts() {
        ifstream fin(accountsFile);
        string line;
        while(getline(fin, line)) {
            if(!line.empty())
                accounts.push_back(Account::deserialize(line));
        }
    }
    void saveTransactions() {
        ofstream fout(transactionsFile);
        for(auto &acc : accounts) {
            for(auto &t : acc.getTransactions()) {
                fout << acc.getAccNo() << "|" << t.serialize() << endl;
            }
        }
    }
    void loadTransactions() {
        ifstream fin(transactionsFile);
        string line;

        while(getline(fin, line)) {
            stringstream ss(line);
            string token;
            vector<string> parts;

            while(getline(ss, token, '|'))
                parts.push_back(token);

            int accNo = stoi(parts[0]);
            Account* acc = findAccount(accNo);

            if(acc) {
                Transaction t(stoi(parts[1]), parts[2], stod(parts[3]));
                acc->getTransactions().push_back(t);
            }
        }
    }
    void menu() {
        int choice;
        do {
            cout << "\nBanking System\n";
            cout << "1. Create Account\n";
            cout << "2. Deposit\n";
            cout << "3. Withdraw\n";
            cout << "4. Check Balance\n";
            cout << "5. Last 5 Transactions\n";
            cout << "6. Total Money (Admin)\n";
            cout << "7. Exit\n";
            cout << "Choice: ";
            cin >> choice;

            switch(choice) {
                case 1: createAccount(); 
                break;
                case 2: depositToAccount(); 
                break;
                case 3: withdrawFromAccount(); 
                break;
                case 4: checkBalance(); 
                break;
                case 5: showTransactions(); 
                break;
                case 6: totalMoney(); break;
                case 7: cout << "Exiting\n"; 
                break;
                default: cout << "Invalid choice\n";
            }
        } while(choice != 7);
    }
};
int main() {
    BankSystem bank;
    bank.menu();
    return 0;
}