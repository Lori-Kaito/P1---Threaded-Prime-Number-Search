#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <thread>
#include <mutex>

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

// ==================== Worker ====================
void primeWorker(int start, int end, int id, vector<string> &results, mutex &resMutex) {
    vector<string> localResults;

    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            stringstream ss;
            ss << "[Thread: " << id << " | Time: " << getTimestamp() << "] Found prime: " << i;
            localResults.push_back(ss.str());
        }
    }

    // Push results into shared vector
    lock_guard<mutex> lock(resMutex);
    results.insert(results.end(), localResults.begin(), localResults.end());
}

// ==================== Main ====================
int main() {
    cout << "Run started at: " << getTimestamp() << endl;

    readConfig("config.txt");

    cout << "\n[Variant 2] Multi-threaded prime search with batch printing and range division\n";

    vector<thread> threads;
    vector<string> results;
    mutex resMutex;

    int numbersPerThread = MAX_NUM / THREADS;
    
    // Create threads with divided ranges
    for (int i = 0; i < THREADS; i++) {
        int start = (i * numbersPerThread) + 1;
        int end = (i == THREADS - 1) ? MAX_NUM : ((i + 1) * numbersPerThread);
        
        // Ensure start is at least 2 (first prime number)
        if (start < 2) start = 2;
        
        threads.push_back(thread(primeWorker, start, end, i + 1, ref(results), ref(resMutex)));
    }

    // Wait for all threads to complete
    for (auto &t : threads) {
        t.join();
    }

    // Print all results after threads finish
    cout << "\n=== All primes found (" << results.size() << " numbers) ===\n";
    for (const auto &result : results) {
        cout << result << endl;
    }

    cout << "\nRun finished at: " << getTimestamp() << endl;
    return 0;
}