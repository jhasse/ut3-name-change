#include <windows.h>
#include <iostream>
#include <string>
#include <shlobj.h>
#include <vector>
#include <sstream>
#include <fstream>

#pragma comment(lib, "shell32.lib")

class PauseAtExit {
public:
	~PauseAtExit() {
		system("pause");
	}
};

int main() {
	PauseAtExit _;

	std::vector<char> my_documents(MAX_PATH);
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, &my_documents[0]);
	if (result != S_OK) {
		std::cout << "Error: " << result << "\n";
		return EXIT_FAILURE;
	}

	const std::string saveDataPath = "\\My Games\\Unreal Tournament 3\\UTGame\\SaveData\\";
	if (!std::ifstream(std::string(&my_documents[0]) + saveDataPath + "Player.ue3profile")) {
		std::cout << "Please install and start Unreal Tournament 3 first!" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Enter your Unreal Tournament 3 multiplayer name: " << std::flush;
	std::string name;
	std::cin >> name;
	std::stringstream sstream;
	sstream << &my_documents[0] << "\\My Games\\Unreal Tournament 3\\UTGame\\Config\\UTGame.ini";
	auto iniName = sstream.str();
	std::ifstream fin(iniName);
	if (!fin) {
		std::cerr << "File not found: " << sstream.str() << std::endl;
		return EXIT_FAILURE;
	}
	sstream << ".bak";
	std::ofstream fout(sstream.str());

	bool success = false;
	while (fin) {
		std::string line;
		std::getline(fin, line);
		fout << line << std::endl;
	}

	fin.close();
	fout.close();
	fin.open(sstream.str());
	fout.open(iniName);
	while (fin) {
		std::string line;
		std::getline(fin, line);
		if (line == "PlayerNames=") {
			success = true;
			fout << "PlayerNames=" << name << std::endl;
		}
		fout << line << std::endl;
	}

	if (!success) {
		std::cout << "Error: Couldn't set player name in UTGame.ini." << std::endl;
		return EXIT_FAILURE;
	}

	std::stringstream cmd;
	cmd << "copy \""
	    << &my_documents[0] << saveDataPath << "Player.ue3profile\" \""
	    << &my_documents[0] << saveDataPath << name << ".ue3profile\"";
	std::cout << std::endl << cmd.str() << std::endl << std::endl;
	if (system(cmd.str().c_str())) {
		std::cout << "Error: Copying profile file failed." << std::endl;
	} else {
		std::cout << "\nSuccess!" << std::endl;
	}
}
