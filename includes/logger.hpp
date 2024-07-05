#pragma once

#include <string>
#include <vector>

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
	std::string logMessage;
	LogLevel logLevel;

	Log(std::string logMessage, LogLevel logLevel);
};

/// <summary>
/// A class used to log various messages and display them to the user
/// </summary>
class Logger
{
public:
	/// <summary>
	/// Adds a new info log message
	/// </summary>
	/// <param name="message">The message to log</param>
	static void logInfo(std::string message);

	/// <summary>
	/// Adds a new warning log message
	/// </summary>
	/// <param name="message">The message to log</param>
	static void logWarning(std::string message);

	/// <summary>
	/// Adds a new error log message
	/// </summary>
	/// <param name="message">The message to log</param>
	static void logError(std::string message);

	/// <summary>
	/// Adds a new debug log message
	/// </summary>
	/// <param name="message">The message to log</param>
	static void logDebug(std::string message);

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
};