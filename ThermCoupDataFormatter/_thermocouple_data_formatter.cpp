#include <fstream>
#include <filesystem>
#include <iostream>
#include <regex>


inline void fShowErrMsgAndExit(const char* const errorMessage, const std::filesystem::path& filePath);
inline std::string toSecsFromHHMMSS(const std::string& time, const std::regex& specificPattern = std::regex("[0-9]+"));
inline std::string getTimeWithBias(const std::string& time, const std::string& timeBias);


int main(int argc, char** argv)
{
	using fsp = std::filesystem::path;

	fsp inPath, outPath;
	std::fstream inFile, outFile;

	const std::regex timePattern("[0-9]+:[0-9]{2}:[0-9]{2}");
	const std::regex valPattern("[0-9]+[\.][0-9]+");
	std::smatch match;
	std::string dataStr{ "" },
				timeStr{ "" },
				valStr{ "" };

	for (int i{ 1 }; i < argc; ++i)
	{
		inPath = fsp(argv[i]);
		outPath = fsp(
			inPath.parent_path().string() + '\\' + inPath.stem().string()
			+ "_time-val_format" + inPath.extension().string()
		);

		inFile.open(inPath, std::fstream::in);
		if (!inFile.is_open())
		{
			fShowErrMsgAndExit("Can't open the file", inPath);
		}
		outFile.open(outPath, std::fstream::out | std::fstream::app);
		if (!outFile.is_open())
		{
			inFile.close();
			fShowErrMsgAndExit("Can't create or open the output file", outPath);
		}

		// Pass the first line
		std::getline(inFile, dataStr);
		// Get a time bias value to start the time from 0
		std::getline(inFile, dataStr);
		timeStr = std::regex_search(dataStr, match, timePattern) ? match[0] : std::string();
		std::string timeBias{ toSecsFromHHMMSS(timeStr) };
		// TODO: Data formatting loop
		do
		{
			timeStr = std::regex_search(dataStr, match, timePattern) ? match[0] : std::string();
			valStr = std::regex_search(dataStr, match, valPattern) ? match[0] : std::string();
			outFile << getTimeWithBias(toSecsFromHHMMSS(timeStr), timeBias) << '\t' << valStr << '\n';
		} while (std::getline(inFile, dataStr));

		inFile.close();
		outFile.close();
	}

	return 0;
}


void fShowErrMsgAndExit(const char* const errorMessage, const std::filesystem::path& filePath)
{
	std::cout << errorMessage << char(32) << filePath << '\n';
	system("pause");
	exit(-1);
}

std::string toSecsFromHHMMSS(const std::string& time, const std::regex& specificPattern)
{
	int totalTime{ 0 },
		multiplicator{ 3600 };
	std::sregex_iterator it(time.begin(), time.end(), specificPattern),
						 end;
	while (it != end)
	{
		totalTime += (std::stoi((*it)[0]) * multiplicator);
		multiplicator /= 60;
		++it;
	}

	return std::to_string(totalTime);
}

inline std::string getTimeWithBias(const std::string& time, const std::string& timeBias)
{
	return std::to_string(
		std::stoi(time) - std::stoi(timeBias)
	);
}