#include "Utility.cpp"
#include <string>
#include <vector>
#include <unordered_set>
#include <cctype>
#include <limits>
using namespace std;

class Child : public Utility {
public:
    string getMasterDataFilePath() override {
        return "./reduced-master.csv";
    }

    string extractOtp(const string& message) override {
        if (message.empty())
            return "";

        // -------------------------------
        // 1. Normalize + lowercase
        // -------------------------------
        string text;
        text.reserve(message.size());

        for (int i = 0; i < message.size(); i++) {
            char c = tolower(message[i]);

            // Remove "<#>"
            if (c == '<' && i + 2 < message.size() &&
                message[i + 1] == '#' && message[i + 2] == '>') {
                i += 2;
                continue;
            }

            // Remove "g-" or "g " before digits
            if (c == 'g' && i + 1 < message.size() &&
                (message[i + 1] == '-' || message[i + 1] == ' ')) {
                if (i + 2 < message.size() && isdigit(message[i + 2])) {
                    i++;
                    continue;
                }
            }

            // Remove dash between digits (908-618 → 908618)
            if (c == '-' && !text.empty() &&
                i + 1 < message.size() &&
                isdigit(text.back()) && isdigit(message[i + 1])) {
                continue;
            }

            text.push_back(c);
        }

        // -------------------------------
        // 2. Tokenize (non-word split)
        // -------------------------------
        vector<string> tokens;
        string curr;

        for (char c : text) {
            if (isalnum(c) || c == '_') {
                curr.push_back(c);
            } else {
                if (!curr.empty()) {
                    tokens.push_back(curr);
                    curr.clear();
                }
            }
        }
        if (!curr.empty())
            tokens.push_back(curr);

        // -------------------------------
        // 3. Keyword sets
        // -------------------------------
        unordered_set<string> otpKeywords = {
            "otp", "code", "verification", "verify",
            "authentication", "authorization",
            "password", "login", "passcode", "secure"
        };

        unordered_set<string> amountKeywords = {
            "rs", "inr", "rupees"
        };

        unordered_set<string> cardKeywords = {
            "card", "account", "ac", "a",
            "ending", "xxxx", "id"
        };

        // -------------------------------
        // 4. Find OTP keyword positions
        // -------------------------------
        vector<int> keywordPositions;
        for (int i = 0; i < tokens.size(); i++) {
            if (otpKeywords.count(tokens[i]))
                keywordPositions.push_back(i);
        }

        if (keywordPositions.empty())
            return "";

        // -------------------------------
        // 5. Score OTP candidates
        // -------------------------------
        int MAX_WINDOW = tokens.size();
        int bestScore = numeric_limits<int>::min();
        string bestOtp = "";

        for (int i = 0; i < tokens.size(); i++) {
            const string& t = tokens[i];

            // ---- Must be numeric & length 4–8
            if (t.length() < 4 || t.length() > 8)
                continue;

            bool allDigits = true;
            for (char c : t) {
                if (!isdigit(c)) {
                    allDigits = false;
                    break;
                }
            }
            if (!allDigits)
                continue;

            // ---- Ignore amounts
            if (i > 0 && amountKeywords.count(tokens[i - 1]))
                continue;

            // ---- Ignore card/account numbers
            bool isCardRelated = false;
            for (int off = -2; off <= 0; off++) {
                int pos = i + off;
                if (pos >= 0 && pos < tokens.size() &&
                    cardKeywords.count(tokens[pos])) {
                    isCardRelated = true;
                    break;
                }
            }
            if (isCardRelated)
                continue;

            // ---- Score by distance to OTP keyword
            int minDist = 1e9;
            for (int kp : keywordPositions)
                minDist = min(minDist, abs(i - kp));

            int score = MAX_WINDOW - minDist;

            // Prefer OTP after keyword
            for (int kp : keywordPositions) {
                if (i > kp) {
                    score++;
                    break;
                }
            }

            if (score > bestScore) {
                bestScore = score;
                bestOtp = t;
            }
        }

        return bestOtp;
    }
};
