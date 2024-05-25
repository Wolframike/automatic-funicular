function processFile() {
	const fileInput = document.getElementById('jsonFile');
	const myTeamNumberInput = document.getElementById('myTeamNumber');
	const outputDiv = document.getElementById('output');
	const copyButton = document.getElementById('copyButton');

	if (!fileInput.files.length || !myTeamNumberInput.value) {
		alert('Please upload a JSON file and enter your team number.');
		return;
	}

	const myTeamNumber = parseInt(myTeamNumberInput.value);
	const file = fileInput.files[0];
	const reader = new FileReader();

	reader.onload = function(event) {
		const json = JSON.parse(event.target.result);

		const teamNames = ["", ""];
		teamNames[myTeamNumber - 1] = "ME";
		teamNames[2 - myTeamNumber] = "OP";

		const mapName = json["map"]["name"];
		const date = json["startDate"];
		const dateTrimmed = date.substr(0, 10);

		let myAttack = [0, 0], myDefense = [0, 0];
		let attackPistolResult = "", defensePistolResult = "";

		json["stats"]["rounds"].forEach((round, i) => {
			const winningTeamNumber = round["winningTeamNumber"];
			const attackingTeamNumber = round["attackingTeamNumber"];
			const isAttackWin = winningTeamNumber === attackingTeamNumber;
			const isAttacking = attackingTeamNumber === myTeamNumber;

			if (winningTeamNumber === myTeamNumber) {
				if (isAttackWin) {
					++myAttack[0];
				} else {
					++myDefense[0];
				}
			}

			if (isAttacking) {
				++myAttack[1];
			} else {
				++myDefense[1];
			}

			if (i === 0 || i === 12) {
				const pistolResult = winningTeamNumber === myTeamNumber ? "Win" : "Lose";
				if (isAttacking) {
					attackPistolResult = pistolResult;
				} else {
					defensePistolResult = pistolResult;
				}
			}
		});

		const totalRounds = myAttack[1] + myDefense[1];
		const totalWins = myAttack[0] + myDefense[0];
		const totalLosses = totalRounds - totalWins;

		let result = "";
		if (totalWins > totalLosses) {
			result = "Win";
		} else if (totalWins < totalLosses) {
			result = "Lose";
		} else {
			result = "Draw";
		}

		const totalWinRate = (totalWins / totalRounds * 100).toFixed(2);
		const myAttackWinRate = (myAttack[0] / myAttack[1] * 100).toFixed(2);
		const myDefenseWinRate = (myDefense[0] / myDefense[1] * 100).toFixed(2);

		const output = `
			${dateTrimmed} ${mapName} OPPONENT ${result}, 
			${totalWinRate}% 
			${totalWins} ${totalLosses}, 
			${myAttack[0]} ${myAttack[1] - myAttack[0]}, 
			${myAttackWinRate}% 
			${myDefense[0]} ${myDefense[1] - myDefense[0]}, 
			${myDefenseWinRate}% 
			${attackPistolResult} ${defensePistolResult}
		`.replace(/\s+/g, ' ').trim();

		outputDiv.innerText = output;
		copyButton.style.display = 'inline-block';
	};

	reader.readAsText(file);
}

function copyOutput() {
	const outputDiv = document.getElementById('output');
	const copyText = outputDiv.innerText;

	navigator.clipboard.writeText(copyText).then(() => {
		alert('Output copied to clipboard!');
	}).catch(err => {
		alert('Failed to copy output: ', err);
	});
}
