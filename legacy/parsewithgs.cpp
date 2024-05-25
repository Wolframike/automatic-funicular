#include <windows.h>
#include <commdlg.h>
#include <string>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip> // for std::setprecision

using json = nlohmann::json;

std::string GetFileName() {
    OPENFILENAME ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "JSON Files\0*.json\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        return szFile;
    } else {
        return "";
    }
}

int GetSZTeamNumber() {
    int teamNumber = 0;
    std::string input;
    while (teamNumber != 1 && teamNumber != 2) {
        input = "Enter SZ team number (1 or 2):";
        int result = MessageBox(NULL, input.c_str(), "Input", MB_OKCANCEL);
        if (result == IDCANCEL) {
            exit(0);
        }
        std::stringstream ss(input);
        ss >> teamNumber;
    }
    return teamNumber;
}

void ShowResults(const std::string& results) {
    MessageBox(NULL, results.c_str(), "Results", MB_OK);
}

int main() {
    std::vector<std::string> teamNames(2);
    std::string fileName = GetFileName();

    if (fileName.empty()) {
        return 1;
    }

    std::ifstream file(fileName);
    if (!file) {
        return 1;
    }

    int szTeamNumber = GetSZTeamNumber();
    teamNames[szTeamNumber - 1] = "SZ";
    teamNames[2 - szTeamNumber] = "OP";

    json j;
    file >> j;

    std::string mapName = j["map"]["name"];
    std::string date = j["startDate"];
    std::string dateTrimmed = date.substr(0, 10);

    std::pair<int, int> szAttack(0, 0), szDefense(0, 0);
    std::string attackPistolResult, defensePistolResult;

    for (size_t i = 0; i < j["stats"]["rounds"].size(); ++i) {
        const auto& round = j["stats"]["rounds"][i];
        int winningTeamNumber = round["winningTeamNumber"];
        int attackingTeamNumber = round["attackingTeamNumber"];
        bool isAttackWin = winningTeamNumber == attackingTeamNumber;

        if (winningTeamNumber == szTeamNumber) {
            if (isAttackWin) {
                ++szAttack.first;
            } else {
                ++szDefense.first;
            }
        }

        if (attackingTeamNumber == szTeamNumber) {
            ++szAttack.second;
        } else {
            ++szDefense.second;
        }

        if (i == 0 || i == 12) {
            std::string& pistolResult = (i == 0) ? attackPistolResult : defensePistolResult;
            pistolResult = (winningTeamNumber == szTeamNumber) ? "Win" : "Lose";
        }
    }

    int totalRounds = szAttack.second + szDefense.second;
    int totalWins = szAttack.first + szDefense.first;
    int totalLosses = totalRounds - totalWins;

    std::string result;
    if (totalWins > totalLosses) {
        result = "Win";
    } else if (totalWins < totalLosses) {
        result = "Lose";
    } else {
        result = "Draw";
    }

    double totalWinRate = (double)totalWins / totalRounds * 100;
    double szAttackWinRate = (double)szAttack.first / szAttack.second * 100;
    double szDefenseWinRate = (double)szDefense.first / szDefense.second * 100;

    std::ostringstream results;
    results << dateTrimmed << ", " << mapName << ", OPPONENT, " << result << ", "
        << std::setprecision(2) << totalWinRate << "%, "
        << totalWins << ", " << totalLosses << ", "
        << szAttack.first << ", " << szAttack.second - szAttack.first << ", "
        << std::setprecision(2) << szAttackWinRate << "%, "
        << szDefense.first << ", " << szDefense.second - szDefense.first << ", "
        << std::setprecision(2) << szDefenseWinRate << "%, "
        << attackPistolResult << ", " << defensePistolResult;

    ShowResults(results.str());

    return 0;
}
