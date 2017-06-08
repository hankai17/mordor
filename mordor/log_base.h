#ifndef __MORDOR_LOG_BASE_H__
#define __MORDOR_LOG_BASE_H__
// Copyright (c) 2009 - Mozy, Inc.

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

// This is done in mordor/predef.h, but it cannot be directly included because of _WIN32_WINNT definition mismatches
// Take things out of the preprocessor, and put into the global namespace
// From WinGDI.h: #define ERROR 0
#ifdef ERROR
#undef ERROR
enum {
    ERROR = 0
};
#endif

namespace Mordor {

    class Logger;
    class LogSink;

    class LoggerIterator;

    class Stream;

    /// Static class to gain access to and configure global logger instances

    /// The logging framework is made up of three main classes: Log, Logger, and
    /// LogSink.  LogSinks are where log messages go.  Loggers are used to break
    /// up the logging into logical units, and are set up in a hierarchy.  Every
    /// Logger has a name that implies its location in the hieararchy: The name
    /// is split on colons, and it is a child Logger of the Logger with everything
    /// except the last component.  Intermediate loggers are implicitly created, if
    /// necessary.  When you add a LogSink to a Logger, all messages generated by
    /// that logger and all of its children are sent to that LogSink.  Each Logger
    /// has it's own log Level that it is enabled for.  Each higher level is a
    /// superset of the prior level.  So, for example, the root logger can have a
    /// StdoutLogSink, and set for DEBUG level, and a logger named
    /// mordor:http:client can have a FileLogSink and be set for WARNING level.  In
    /// that case, a log message sent to mordor:http:client at INFO level would be
    /// dropped, but at ERROR level would go to both the file, and stdout (that it
    /// inherited from the root logger).  A message sent to mordor:streams:ssl at
    /// DEBUG level would only go to stdout.
    ///
    /// By default, all Loggers have no LogSinks, and are set at INFO level.
    ///
    /// In practice, it is easiest to use the log.* ConfigVars to configure
    /// logging.  There is a ConfigVar for each standard LogSink to enable it (on
    /// the root Logger), and a ConfigVar for each log level.  Each log level
    /// ConfigVar is a regex of which Loggers should be enabled at that level.  A
    /// Logger will be set to the highest level that it matches: i.e. if
    /// log.debugmask=mordor:http:client, and log.tracemask=.*, mordor:http:client
    /// will be set to TRACE.  Note that if any of the log level ConfigVars change,
    /// it will wipe out any manually configured logging levels (so is incompatible
    /// with manually managing log levels).  The log sink ConfigVars are not
    /// incompatible with manually managing your own LogSinks.

    /// @sa LogMacros
    class Log
    {
    private:
        Log();

    public:
        /// The level of a log message
        enum Level {
            NONE,

            /// The application cannot continue
            FATAL,

            /// An error occurred; it cannot locally be recovered from, but may be
            /// recoverable from a more general context
            ERROR,

            /// An error occured that was ignored/recovered, but may be useful
            /// to know the error happened
            WARNING,

            /// A normal, but significant event occurred
            INFO,

            /// A somewhat significant event occurred
            VERBOSE,

            /// Normally only useful for debugging, logs most calls a component
            /// makes
            DEBUG,

            /// Normally only useful for debugging, logs everything under the sun,
            /// including every call a component makes, and possibly details about
            /// internal state
            TRACE,
        };

        /// Find (or create) a logger with the specified name
        static boost::shared_ptr<Logger> lookup(const std::string &name);

        /// Call dg for each registered Logger.
        ///
        /// This may include implicitly created intermediate loggers.
        static void visit(boost::function<void(boost::shared_ptr<Logger>)> dg);

        /// Return the root of the Logger hierarchy
        static boost::shared_ptr<Logger> root();
    };
}


#endif