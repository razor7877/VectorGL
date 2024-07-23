#include "logger.hpp"

Log::Log(std::string logMessage, LogLevel logLevel, std::string filename)
{
	this->logMessage = logMessage;
	this->logLevel = logLevel;
	this->filename = filename;
	this->timestamp = std::chrono::system_clock::now();
}

std::vector<Log> Logger::logMessages;

void Logger::logInfo(std::string message, std::string filename)
{
	Logger::addLog(Log("Info: " + message, LogLevel::LOG_INFO, filename));
}

void Logger::logWarning(std::string message, std::string filename)
{
	Logger::addLog(Log("Warning: " + message, LogLevel::LOG_WARNING, filename));
}

void Logger::logError(std::string message, std::string filename)
{
	Logger::addLog(Log("Error: " + message, LogLevel::LOG_ERROR, filename));
}

void Logger::logDebug(std::string message, std::string filename)
{
	Logger::addLog(Log("Debug: " + message, LogLevel::LOG_DEBUG, filename));
}

std::vector<Log> Logger::getLogs()
{
	return Logger::logMessages;
}

std::vector<Log> Logger::getLogs(LogLevel logLevel)
{
	std::vector<Log> filteredLogs = std::vector<Log>();

	for (Log log : Logger::logMessages)
		if (log.logLevel == logLevel)
			filteredLogs.push_back(log);

	return filteredLogs;
}

void Logger::clearLogs()
{
	Logger::logMessages.clear();
}

void Logger::addLog(Log log)
{
	Logger::logMessages.push_back(log);

	if (Logger::logMessages.size() > Logger::MAX_LOGS)
		Logger::logMessages.erase(Logger::logMessages.begin(), Logger::logMessages.begin() + Logger::LOGS_REMOVED_ON_LIMIT_REACHED);
}