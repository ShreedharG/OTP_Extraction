#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct Result {
    string message;
    string extractedOtp;
    string expectedOtp;
};

class Utility {
public:
    virtual ~Utility() = default;

    virtual string getMasterDataFilePath() = 0;
    virtual string extractOtp(const string& message) = 0;

    vector<vector<string>> readFileData() {
        vector<vector<string>> dataRows;
        string filePath = getMasterDataFilePath();

        if (filePath.empty()) {
            cerr << "Error: Master data file path is not provided." << endl;
            return dataRows;
        }

        ifstream file(filePath);
        if (!file.is_open()) {
            cerr << "Error: Could not open file: " << filePath << endl;
            return dataRows;
        }

        string header;
        getline(file, header); 

        vector<string> row;
        while (_parseCsvRow(file, row)) {
            if (!row.empty()) {
                dataRows.push_back(row);
            }
        }
        return dataRows;
    }

    void writeResults(const vector<Result>& results) {
        string filePath = getMasterDataFilePath();
        string folderPath = ".";
        size_t lastSlash = filePath.find_last_of("/\\");
        if (lastSlash != string::npos) {
            folderPath = filePath.substr(0, lastSlash);
        }

        ofstream outputFile(folderPath + "/output.csv");
        ofstream failedFile(folderPath + "/failedCases.csv");

        if (!outputFile.is_open() || !failedFile.is_open()) {
            cerr << "Error: Could not open output files for writing." << endl;
            return;
        }

        outputFile << "Message,Extracted,Expected\n";
        failedFile << "Message,Extracted,Expected\n";

        int failCases = 0;

        for (const auto& result : results) {
            bool isMatch = (result.extractedOtp == result.expectedOtp);
            string row_str = _escapeCsv(result.message) + "," +
                             _escapeCsv(result.extractedOtp) + "," +
                             _escapeCsv(result.expectedOtp) + "\n";
            outputFile << row_str;
            if (!isMatch) {
                failCases++;
                failedFile << row_str;
            }
        }

        double accuracy = results.empty() ? 0.0 : (double)(results.size() - failCases) / results.size() * 100.0;
        cout << "Total Cases: " << results.size() << endl;
        cout << "Failed Cases: " << failCases << endl;
        cout << "Accuracy: " << accuracy << "%" << endl;
    }

private:
    bool _parseCsvRow(ifstream& file, vector<string>& row) {
        row.clear();
        string field;
        char c;
        bool inQuotes = false;

        if (file.peek() == EOF) return false;

        while (file.get(c)) {
            if (inQuotes) {
                if (c == '"') {
                    if (file.peek() == '"') {
                        field += '"'; 
                        file.get(); 
                    } else {
                        inQuotes = false; 
                    }
                } else {
                    field += c;
                }
            } else { 
                if (c == '"') {
                    inQuotes = true;
                } else if (c == ',') {
                    row.push_back(field);
                    field.clear();
                } else if (c == '\n' || c == '\r') {
                    if (c == '\r' && file.peek() == '\n') {
                        file.get();
                    }
                    row.push_back(field);
                    return true; 
                } else {
                    field += c;
                }
            }
        }
        row.push_back(field);
        return true;
    }

    string _escapeCsv(const string& field) {
        if (field.find_first_of(",\"\n\r") != string::npos) {
            string escaped = "\"";
            for (char c : field) {
                if (c == '"') escaped += "\"\"";
                else escaped += c;
            }
            escaped += "\"";
            return escaped;
        }
        return field;
    }
};