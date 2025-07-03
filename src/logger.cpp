#include <algorithm>
#include <iterator>

#include "logger.hpp"

std::vector<Log> Logger::logMessages;

std::set<std::string> Logger::sourceFiles;

Log::Log(const std::string &logMessage, LogLevel logLevel, const std::string &filename)
{
	this->logMessage = logMessage;
	this->logLevel = logLevel;
	this->filename = filename;
	this->timestamp = std::chrono::system_clock::now();
}

void Logger::logInfo(const std::string& message, const std::string& filename)
{
	Logger::addLog(Log("Info: " + message, LogLevel::LOG_INFO, filename));
}

void Logger::logWarning(const std::string& message, const std::string& filename)
{
	Logger::addLog(Log("Warning: " + message, LogLevel::LOG_WARNING, filename));
}

void Logger::logError(const std::string& message, const std::string &filename)
{
	Logger::addLog(Log("Error: " + message, LogLevel::LOG_ERROR, filename));
}

void Logger::logDebug(const std::string& message, const std::string& filename)
{
	Logger::addLog(Log("Debug: " + message, LogLevel::LOG_DEBUG, filename));
}

std::vector<Log> Logger::getLogs()
{
	return Logger::logMessages;
}

int Logger::getLogCount()
{
	return Logger::logMessages.size();
}

std::vector<Log> Logger::getLogsByLevel(LogLevel logLevel)
{
	std::vector<Log> filteredLogs;

	std::copy_if(
		Logger::logMessages.begin(),
		Logger::logMessages.end(),
		std::back_inserter(filteredLogs),
		[&logLevel](const Log& log)
		{
			return log.logLevel == logLevel;
		}
	);

	return filteredLogs;
}

std::vector<Log> Logger::getLogsFromFiles(std::vector<std::string> files)
{
	std::vector<Log> filteredLogs;

	std::copy_if(
		Logger::logMessages.begin(),
		Logger::logMessages.end(),
		std::back_inserter(filteredLogs),
		[&files](const Log& log)
		{
			return std::find(files.begin(), files.end(), log.filename) != files.end();
		}
	);

	return filteredLogs;
}

std::vector<Log> Logger::getFilteredLogs(std::set<LogLevel> logLevels, std::set<std::string> files)
{
	std::vector<Log> filteredLogs;

	std::copy_if(
		Logger::logMessages.begin(),
		Logger::logMessages.end(),
		std::back_inserter(filteredLogs),
		[&logLevels, &files](const Log& log)
		{
			const bool inSourceFiles = files.empty() || files.find(log.filename) != files.end();
			const bool inLogLevels = logLevels.find(log.logLevel) != logLevels.end();
			return inSourceFiles && inLogLevels;
		}
	);

	return filteredLogs;
}

std::vector<std::string> Logger::getSourceFiles()
{
	return std::vector(Logger::sourceFiles.begin(), Logger::sourceFiles.end());
}

void Logger::clearLogs()
{
	Logger::logMessages.clear();
	Logger::sourceFiles.clear();
}

void Logger::addLog(const Log& log)
{
	bool previousLogIsSame = false;

	if (!Logger::logMessages.empty())
	{
		Log& lastLog = Logger::logMessages.back();

		if (lastLog.filename == log.filename && lastLog.logLevel == log.logLevel && lastLog.logMessage == log.logMessage)
		{
			lastLog.logCount++;
			previousLogIsSame = true;
		}
	}
	
	if (!previousLogIsSame)
		Logger::logMessages.push_back(log);

	// Track the source files from which we got logs
	Logger::sourceFiles.insert(log.filename);

	// Erase old logs if we reached the max limit
	if (Logger::logMessages.size() > Logger::MAX_LOGS)
		Logger::logMessages.erase(Logger::logMessages.begin(), Logger::logMessages.begin() + Logger::LOGS_REMOVED_ON_LIMIT_REACHED);
}