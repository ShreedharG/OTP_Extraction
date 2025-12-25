#include "Child.cpp"

using namespace std;

int main() {
    Child obj;

    vector<vector<string>> csvData = obj.readFileData();
    vector<Result> processedResults;

    for (const auto& row : csvData) {
        if (row.size() >= 2) {
            string message = row[0];
            string expectedOtp = row[1];

            string extractedOtp = obj.extractOtp(message);

            processedResults.push_back({message, extractedOtp, expectedOtp});
        }
    }

    obj.writeResults(processedResults);

    return 0;
}