#pragma once

#include <string>
#include <vector>
#include <chrono>

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
	/// Returns all the logs filtered by the given log level
	/// </summary>
	/// <param name="logLevel">The log level to filter the messages with</param>
	static std::vector<Log> getLogs(LogLevel logLevel);

	/// <summary>
	/// Clears the list of log messages
	/// </summary>
	static void clearLogs();

private:
	static std::vector<Log> logMessages;

	static void addLog(Log log);
};