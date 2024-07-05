#include "logger.hpp"

Log::Log(std::string logMessage, LogLevel logLevel)
{
	this->logMessage = logMessage;
	this->logLevel = logLevel;
}

std::vector<Log> Logger::logMessages;

void Logger::logInfo(std::string message)
{
	Logger::logMessages.push_back(Log("Info: " + message, LogLevel::LOG_INFO));
}

void Logger::logWarning(std::string message)
{
	Logger::logMessages.push_back(Log("Warning: " + message, LogLevel::LOG_WARNING));
}

void Logger::logError(std::string message)
{
	Logger::logMessages.push_back(Log("Error: " + message, LogLevel::LOG_ERROR));
}

void Logger::logDebug(std::string message)
{
	Logger::logMessages.push_back(Log("Debug: " + message, LogLevel::LOG_DEBUG));
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