#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <iomanip>

using namespace std;

// Global config values
int THREADS;
int MAX_NUM;

// ==================== Read Config ====================
void readConfig(const string &filename = "config.txt") {
    ifstream configFile(filename);
    if (!configFile.is_open()) {
        cerr << "Error: Could not open " << filename << ". Program will exit." << endl;
        exit(1);
    }

    string line;
    bool threadsSet = false, maxNumSet = false;

    while (getline(configFile, line)) {
        stringstream ss(line);
        string key, value;
        if (getline(ss, key, '=') && getline(ss, value)) {
            if (key == "threads") {
                THREADS = stoi(value);
                threadsSet = true;
            } else if (key == "max_number") {
                MAX_NUM = stoi(value);
                maxNumSet = true;
            }
        }
    }

    if (!threadsSet || !maxNumSet) {
        cerr << "Error: Missing required values in config file. Program will exit." << endl;
        exit(1);
    }

    cout << "Config Loaded -> Threads: " << THREADS << ", Max Number: " << MAX_NUM << endl;
}

// ==================== Helpers ====================
string getTimestamp() {
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto epoch = now_ms.time_since_epoch();
    auto value = chrono::duration_cast<chrono::milliseconds>(epoch);
    time_t now_c = chrono::system_clock::to_time_t(now);
    tm local_tm{};
#ifdef _WIN32
    localtime_s(&local_tm, &now_c);
#else
    localtime_r(&now_c, &local_tm);
#endif
    stringstream ss;
    ss << put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
    ss << "." << setfill('0') << setw(3) << (value.count() % 1000);
    return ss.str();
}

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

// ==================== Global Variables ====================
mutex mtx;
int currentNum = 2;
vector<string> primeResults; // Store results with timestamps

// ==================== Worker Function ====================
void worker(int threadId) {
    while (true) {
        int num;
        {
            lock_guard<mutex> lock(mtx);
            if (currentNum > MAX_NUM) break;
            num = currentNum++;
        }
        
        if (isPrime(num)) {
            stringstream ss;
            ss << "[Thread: " << threadId << " | Time: " << getTimestamp() << "] Found prime: " << num;
            
            lock_guard<mutex> lock(mtx);
            primeResults.push_back(ss.str());
            // Print immediately (Variant 3 requirement: Print immediately)
            cout << ss.str() << endl;
        }
    }
}

// ==================== Main ====================
int main() {
    cout << "Run started at: " << getTimestamp() << endl;

    readConfig("config.txt");

    cout << "\n[Variant 3] Multi-threaded prime search with immediate printing and individual number testing\n";

    vector<thread> threads;
    
    // Create threads for individual number testing
    for (int t = 0; t < THREADS; t++) {
        threads.emplace_back(worker, t + 1);
    }

    // Wait for all threads to complete
    for (auto &th : threads) {
        th.join();
    }

    // Print summary
    cout << "\n=== Found " << primeResults.size() << " prime numbers ===" << endl;

    cout << "\nRun finished at: " << getTimestamp() << endl;
    return 0;
}