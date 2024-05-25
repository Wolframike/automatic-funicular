#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

int main() {
	std::vector<std::string> teamNames(2);
	std::ifstream file("scrim.json");

	std::cout << "Team 1: ";
	std::cin >> teamNames[0];
	std::cout << "Team 2: ";
	std::cin >> teamNames[1];

	// Check if file opened successfully
	if (!file) {
		std::cerr << "Failed to open file\n";
		return 1;
	}

	json j;
	file >> j;

	// Get the map name and print it
	std::string mapName = j["map"]["name"];
	std::cout << "map: " << mapName << '\n';

	// Get the date, trim it to only include the date part, and print it
	std::string date = j["startDate"];
	std::string dateTrimmed = date.substr(0, 10);  // Get the first 10 characters of the date string
	std::cout << "date: " << dateTrimmed << '\n';

	// Create a vector to store the winning team names
	std::vector<std::pair<std::string, bool>> teamScores;

	// Iterate over the rounds
	for (const auto& round : j["stats"]["rounds"]) {
		// Add the winning team name to the vector
		teamScores.push_back(std::make_pair(teamNames[round["winningTeamNumber"].get<int>() - 1], round["attackingTeamNumber"] == 1));
	}

	// Print the winning team names
	for (size_t i = 0; i < teamScores.size(); ++i) {
		std::cout
			<< "Round " << i + 1 << ": " << teamScores[i].first
			<< " (" << teamNames[0] << " on " << (teamScores[i].second ? "Attack" : "Defense") << ")\n";
	}

	return 0;
}