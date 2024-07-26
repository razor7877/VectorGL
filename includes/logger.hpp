#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <set>

/// <summary>
/// An enum that stores the various log levels
/// </summary>
enum class LogLevel
{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_DEBUG,
};

/// <summary>
/// Represents a single log message and it's associated data
/// </summary>
struct Log
{
	/// <summary>
	/// The actual message in the log
	/// </summary>
	std::string logMessage;

	/// <summary>
	/// The severity level of the log
	/// </summary>
	LogLevel logLevel;

	/// <summary>
	/// The name of the file that created the log
	/// </summary>
	std::string filename;

	/// <summary>
	/// The timestamp at which the log was created
	/// </summary>
	std::chrono::system_clock::time_point timestamp;

	Log(std::string logMessage, LogLevel logLevel, std::string filename);
};

/// <summary>
/// A class used to log various messages and display them to the user
/// </summary>
class Logger
{
public:
	/// <summary>
	/// The maximum amount of logs that the Logger will store
	/// </summary>
	static constexpr int MAX_LOGS = 1000;

	/// <summary>
	/// How many log messages are deleted once the Logger max logs limit is reached
	/// </summary>
	static constexpr int LOGS_REMOVED_ON_LIMIT_REACHED = 500;

	/// <summary>
	/// Adds a new info log message
	/// </summary>
	/// <param name="message">The message to log</param>
	static void logInfo(std::string message, std::string filename);

	/// <summary>
	/// Adds a new warning log message
	/// </summary>
	/// <param name="message">The message to log</param>
	static void logWarning(std::string message, std::string filename);

	/// <summary>
	/// Adds a new error log message
	/// </summary>
	/// <param name="message">The message to log</param>
	static void logError(std::string message, std::string filename);

	/// <summary>
	/// Adds a new debug log message
	/// </summary>
	/// <param name="message">The message to log</param>
	static void logDebug(std::string message, std::string filename);

	/// <summary>
	/// Returns all the logs
	/// </summary>
	static std::vector<Log> getLogs();

	/// <summary>
	/// Returns how many logs are currently stored
	/// </summary>
	/// <returns>The count of logs</returns>
	static int getLogCount();

	/// <summary>
	/// Returns all the logs filtered by the given log level
	/// </summary>
	/// <param name="logLevel">The selected log level</param>
	static std::vector<Log> getLogsByLevel(LogLevel logLevel);

	/// <summary>
	/// Returns all the logs filtered by the files that created them
	/// </summary>
	/// <param name="files">The selected source files</param>
	/// <returns>A list of logs that were created by these files</returns>
	static std::vector<Log> getLogsFromFiles(std::vector<std::string> files);

	/// <summary>
	/// Returns a list of logs filtered by log levels 
	/// </summary>
	/// <param name="logLevels">The selected log levels</param>
	/// <param name="files">The selected source files</param>
	/// <returns></returns>
	static std::vector<Log> getFilteredLogs(std::set<LogLevel> logLevels, std::set<std::string> files = Logger::sourceFiles);

	/// <summary>
	/// Returns a list of all source files that have created logs
	/// </summary>
	/// <returns>A vector of file names</returns>
	static std::vector<std::string> getSourceFiles();

	/// <summary>
	/// Clears the list of log messages
	/// </summary>
	static void clearLogs();

private:
	/// <summary>
	/// The list of all stored logs
	/// </summary>
	static std::vector<Log> logMessages;

	/// <summary>
	/// A set that contains the list of source files from which logs have been created, used for filters
	/// </summary>
	static std::set<std::string> sourceFiles;

	/// <summary>
	/// Adds a log message, clears old messages if needed etc.
	/// </summary>
	/// <param name="log">The log to be added</param>
	static void addLog(Log log);
};