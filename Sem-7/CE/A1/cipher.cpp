#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <algorithm>
using namespace std;

// This is an abstract function with pure virtual methods
class Cipher 
{
public:
    virtual string encrypt(const string &text, const string &key) = 0;
    virtual string decrypt(const string &text, const string &key) = 0;
    virtual ~Cipher() {}    // Virtual destructor
};

// Caesar: Derived Class 
// Parent Class: Cipher
class Caesar : public Cipher 
{
public:
    string encrypt(const string &text, const string &key) override 
    {
        int k = stoi(key) % 26;
        string res = text;
        for (char &c : res) 
        {
            if (isalpha(c)) 
            {
                char base = isupper(c) ? 'A' : 'a';
                c = (c - base + k + 26) % 26 + base;
            }
        }
        return res;
    }

    string decrypt(const string &text, const string &key) override 
    {
        int k = stoi(key) % 26;
        return encrypt(text, to_string(26 - k));
    }
};

// Vigenere: Derived Class
// Parent Class: Cipher`
class Vigenere : public Cipher 
{
public:
    string encrypt(const string &text, const string &key) override 
    {
        string res;
        int j = 0, klen = key.size();
        for (char c : text) 
        {
            if (isalpha(c)) 
            {
                char base = isupper(c) ? 'A' : 'a';
                int shift = tolower(key[j % klen]) - 'a';
                res.push_back((c - base + shift) % 26 + base);
                j++;
            } 
            else 
            {
                res.push_back(c);
            }
        }
        return res;
    }

    string decrypt(const string &text, const string &key) override 
    {
        string res;
        int j = 0, klen = key.size();
        for (char c : text) 
        {
            if (isalpha(c)) 
            {
                char base = isupper(c) ? 'A' : 'a';
                int shift = tolower(key[j % klen]) - 'a';
                res.push_back((c - base - shift + 26) % 26 + base);
                j++;
            } 
            else 
            {
                res.push_back(c);
            }
        }
        return res;
    }
};

// Playfair: Derived Class
// Parent Class: Cipher
class Playfair : public Cipher 
{
    char keyT[5][5];

    void toLower(string &s) 
    {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
    }

    void removeSpaces(string &s) 
    {
        s.erase(remove(s.begin(), s.end(), ' '), s.end());
    }

    void generateKeyTable(const string &key) 
    {
        int hash[26] = {0};
        int i = 0, j = 0;

        for (char c : key) {
            if (c != 'j' && !hash[c - 'a']) 
            {
                keyT[i][j++] = c;
                hash[c - 'a'] = 1;
                if (j == 5) { i++; j = 0; }
            }
        }
        hash['j' - 'a'] = 1;

        for (int k = 0; k < 26; k++) 
        {
            if (!hash[k]) {
                keyT[i][j++] = 'a' + k;
                if (j == 5) { i++; j = 0; }
            }
        }
    }

    void search(char a, char b, int pos[4]) 
    {
        if (a == 'j') a = 'i';
        if (b == 'j') b = 'i';
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++) 
            {
                if (keyT[i][j] == a) pos[0] = i, pos[1] = j;
                if (keyT[i][j] == b) pos[2] = i, pos[3] = j;
            }
    }

    string processEncrypt(const string &txt) 
    {
        string out = txt;
        int pos[4];
        for (int i = 0; i < txt.size(); i += 2) 
        {
            search(txt[i], txt[i+1], pos);
            if (pos[0] == pos[2]) 
            {
                out[i]   = keyT[pos[0]][(pos[1]+1)%5];
                out[i+1] = keyT[pos[0]][(pos[3]+1)%5];
            } 
            else if (pos[1] == pos[3]) 
            {
                out[i]   = keyT[(pos[0]+1)%5][pos[1]];
                out[i+1] = keyT[(pos[2]+1)%5][pos[1]];
            } 
            else 
            {
                out[i]   = keyT[pos[0]][pos[3]];
                out[i+1] = keyT[pos[2]][pos[1]];
            }
        }
        return out;
    }

    string processDecrypt(const string &txt) 
    {
        string out = txt;
        int pos[4];
        for (int i = 0; i < txt.size(); i += 2) 
        {
            search(txt[i], txt[i+1], pos);
            if (pos[0] == pos[2]) 
            {
                out[i]   = keyT[pos[0]][(pos[1]+4)%5];
                out[i+1] = keyT[pos[0]][(pos[3]+4)%5];
            } 
            else if (pos[1] == pos[3]) 
            {
                out[i]   = keyT[(pos[0]+4)%5][pos[1]];
                out[i+1] = keyT[(pos[2]+4)%5][pos[1]];
            } 
            else 
            {
                out[i]   = keyT[pos[0]][pos[3]];
                out[i+1] = keyT[pos[2]][pos[1]];
            }
        }
        return out;
    }

public:
    string encrypt(const string &text, const string &key) override 
    {
        string tmp = text, k = key;
        toLower(tmp); 
        removeSpaces(tmp);
        toLower(k);   
        removeSpaces(k);
        generateKeyTable(k);
        if (tmp.size()%2) 
        {
            tmp.push_back('z');
        }
        return processEncrypt(tmp);
    }

    string decrypt(const string &text, const string &key) override 
    {
        string tmp = text, k = key;
        toLower(tmp); 
        removeSpaces(tmp);
        toLower(k);   
        removeSpaces(k);
        generateKeyTable(k);
        return processDecrypt(tmp);
    }
};

// Main Driver Function
int main() 
{
    cout << "Select Cipher:\n1. Caesar\n2. Vigenere\n3. Playfair\n";

    int c; 
    cin >> c;
    cout << "Encrypt(1) or Decrypt(2)? ";

    int op; 
    cin >> op;
    cout << "Enter Key: ";
    string key; 
    cin >> key;

    ifstream fin("process_1.txt");
    if (!fin) { cout << "File not found!\n"; return 0; }
    string text((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());

    Cipher *cipher = nullptr;           // Null pointer initialization (Pointer to Base Class)
    if (c == 1) cipher = new Caesar();  
    else if (c == 2) cipher = new Vigenere();
    else if (c == 3) cipher = new Playfair();

    if (!cipher) return 0;

    string result = (op==1) ? cipher->encrypt(text, key) : cipher->decrypt(text, key);
    cout << "\nResult:\n" << result << "\n";

    delete cipher;      // Freeing allocated memory
    return 0;
}
