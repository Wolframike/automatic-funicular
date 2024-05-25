#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip> // for std::setprecision

using json = nlohmann::json;

int main() {
	std::vector<std::string> teamNames(2);
	std::string fileName;

	std::cout << "Enter json file name: ";
	std::cin >> fileName;

	std::ifstream file(fileName);

	int szTeamNumber;
	std::cout << "Enter SZ team number (1 or 2): ";
	std::cin >> szTeamNumber;

	teamNames[szTeamNumber - 1] = "SZ";
	teamNames[2 - szTeamNumber] = "OP";

	if (!file) {
		return 1;
	}

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

	std::cout << dateTrimmed << ", " << mapName << ", OPPONENT, " << result << ", "
		<< std::setprecision(2) << totalWinRate << "%, "
		<< totalWins << ", " << totalLosses << ", "
		<< szAttack.first << ", " << szAttack.second - szAttack.first << ", "
		<< std::setprecision(2) << szAttackWinRate << "%, "
		<< szDefense.first << ", " << szDefense.second - szDefense.first << ", "
		<< std::setprecision(2) << szDefenseWinRate << "%, "
		<< attackPistolResult << ", " << defensePistolResult << '\n';

	return 0;
}