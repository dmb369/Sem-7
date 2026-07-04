#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <algorithm>

using namespace std; 
using namespace chrono;   // For processing time 

// Expected English letter frequencies
double englishFreq[26] = {
    0.08167,0.01492,0.02782,0.04253,0.12702,0.02228,
    0.02015,0.06094,0.06996,0.00153,0.00772,0.04025,
    0.02406,0.06749,0.07507,0.01929,0.00095,0.05987,
    0.06327,0.09056,0.02758,0.00978,0.02360,0.00150,
    0.01974,0.00074
}; 

// Base class for cryptanalysis methods
class Cryptanalysis 
{
public:
    virtual void attack(const string &cipher) = 0; 
    virtual ~Cryptanalysis() {}
}; 

// Caesar Analysis
class CaesarAnalysis : public Cryptanalysis 
{
    // Decrypt Caesar cipher text with given key
    string decrypt(const string &text, int k) 
    {
        string res = text; 
        for (char &c : res) 
        {
            if (isalpha(c)) 
            {
                char base = isupper(c) ? 'A' : 'a'; 
                c = (c  -  base  -  k + 26*10) % 26 + base;  // reverse shift
            }
        }
        return res; 
    }

public:
    // Using chi - squared test
    void attack(const string &cipher) override
    {
        int bestKey = 0;  
        double bestScore = 1e18; 

        for (int k = 0;  k < 26;  k++) 
        {
            string dec = decrypt(cipher, k); 

            // Count letter frequencies
            int count[26] = {0}, total = 0; 
            for (char c : dec) 
                if (isalpha(c)) { count[toupper(c) - 'A']++;  total++;  }

            // Compute chi - squared score
            double score = 0; 
            for (int i = 0;  i < 26;  i++) 
            {
                double obs = (double)count[i]/max(1,total); 
                double exp = englishFreq[i]; 
                score += (obs - exp)*(obs - exp)/exp; 
            }

            // Update best key
            if (score < bestScore) { bestScore=score;  bestKey=k;  }
        }

        cout  <<  "Detected Caesar key: "  <<  bestKey  <<  "\n"; 
        cout  <<  "Plaintext:\n"  <<  decrypt(cipher, bestKey)  <<  "\n"; 
    }
}; 

// Vigenere Analysis 
class VigenereAnalysis : public Cryptanalysis 
{
    // Compute Index of Coincidence (IC)
    double calculateIC(const string &text)
    {
        int count[26]={0}, n=0; 
        for (char c:text) 
        {
            if(isalpha(c))
            { 
                count[toupper(c) - 'A']++;  n++;  
            }
        }

        if (n<=1) return 0; 

        double ic = 0; 

        for(int i=0; i<26; i++) 
        {
            ic+=count[i]*(count[i] - 1); 
        }
        return ic/(double)(n*(n - 1)); 
    }

    // This will load EnglishWords.txt and calculate reference IC
    double getReferenceIC() 
    {
        FILE *f = fopen("EnglishWords.txt", "r");
        if (!f) 
        {
            printf("EnglishWords.txt not found! Using default IC=0.067\n");
            return 0.067; // fallback
        }
    
        char buf[200000];  
        size_t len = fread(buf, 1, sizeof(buf)-1, f);
        buf[len] = '\0';
        fclose(f);
        return calculateIC(buf);
    }

    // Find key length using IC method with reference IC
    int findKeyLength(const string &cipher) 
    {
        double refIC = getReferenceIC();
        double bestDiff = 1e9;
        int bestLen = 1;

        // Test key lengths from 1 to 20
        for (int keyLen = 1; keyLen <= 20; keyLen++) 
        {
            double avgIC = 0;
        
            for (int offset = 0; offset < keyLen; offset++) 
            {
                char subseq[1000]; 
                int k = 0;

                for (int i = offset; cipher[i]; i += keyLen) 
                {
                    if (isalpha(cipher[i])) {
                        subseq[k++] = cipher[i];
                    }
                }
                subseq[k] = '\0';

                if (k > 1) 
                {
                    avgIC += calculateIC(subseq);
                }
            }
            avgIC /= keyLen;

            // Pick keyLen whose IC is closest to reference English IC
            double diff = fabs(avgIC - refIC);
            if (diff < bestDiff) 
            {
                bestDiff = diff;
                bestLen = keyLen;
            }
        }
        return bestLen;
    }

    // This determines a single Vigenère key character by frequency analysis
    char findKeyChar(const string &subseq) 
    {
        double bestScore = 1e18;  
        char bestChar = 'a'; 
        for(int shift=0; shift<26; shift++) 
        {
            int count[26]={0}, total=0; 

            // Decrypt subsequence with current shift
            for(char c:subseq) 
            {
                char dec=((toupper(c) - 'A'  - shift + 26) % 26)+'A'; 
                count[dec - 'A']++;  
                total++; 
            }

            // Chi - squared test
            double score=0; 
            for(int i=0; i<26; i++) 
            {
                double obs = (double)count[i]/max(1,total); 
                double exp = englishFreq[i]; 
                score += (obs - exp)*(obs - exp)/exp; 
            }

            if(score<bestScore) 
            {
                bestScore=score;  bestChar='a'+shift; 
            }
        }
        return bestChar; 
    }

    // Decrypt Vigenère using given key
    string decrypt(const string &text, const string &key) 
    {
        string res;  int j=0,klen=key.size(); 
        for(char c:text)
        {
            if(isalpha(c))
            {
                char base=isupper(c)?'A':'a'; 
                int shift=tolower(key[j%klen]) - 'a'; 
                res.push_back((c - base - shift+26)%26+base); 
                j++; 
            } 
            else 
            {
                res.push_back(c); 
            }
        }
        return res; 
    }

public:
    // Full attack: detect key length → build key → decrypt
    void attack(const string &cipher) override 
    {
        int keyLen = findKeyLength(cipher); 
        cout << "Detected key length: " << keyLen << "\n"; 

        string key; 
        for(int pos = 0; pos<keyLen; pos++)
        {
            string subseq; 
            for(int i=pos; i<cipher.size(); i+=keyLen)
            {
                if(isalpha(cipher[i]))
                {
                    subseq.push_back(cipher[i]); 
                }
            }

            key.push_back(findKeyChar(subseq)); 
        }

        cout << "Detected Vigenere key: " << key << "\n"; 
        cout << "Plaintext:\n" << decrypt(cipher,key) << "\n"; 
    }
}; 

// Main Driver Function
int main() 
{
    cout << "Cryptanalysis Mode\n1. Caesar\n2. Vigenere\n"; 
    int c;  cin>>c; 

    ifstream fin("process_2.txt"); 
    if(!fin) 
    { 
        cout << "File not found!\n";  return 0;  
    }

    string cipher((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>()); 

    Cryptanalysis *analyzer=nullptr; 
    if(c ==1) analyzer=new CaesarAnalysis(); 
    else if(c ==2) analyzer=new VigenereAnalysis(); 

    if(analyzer) 
    {
        auto start = chrono::high_resolution_clock::now();  // start timer

        analyzer -> attack(cipher); 

        auto end = chrono::high_resolution_clock::now();    // end timer
        auto duration = chrono::duration_cast<chrono::milliseconds>(end  -  start); 
        cout  <<  "\nProcessing time: "  <<  duration.count()  <<  " ms\n"; 
    }

    delete analyzer; 
    return 0; 
}
