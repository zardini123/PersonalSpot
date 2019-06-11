const bunyan = require('bunyan');

const method = ErrorHandler.prototype;

const _ = require('private-parts').createKey();

/**
 * Centralized error-handling solution
 * Exports as a singleton
 * @param       {String} logFilePath Path to the error handler's log file
 * @constructor
 */
function ErrorHandler(logFilePath) {
  if (!logFilePath) logFilePath = 'out.log';
  _(this).mainLogger = bunyan.createLogger({
    name: 'PersonalSpot',
    streams: [
      {
        level: 'error',
        path: logFilePath,
      },
      {
        level: 'info',
        stream: process.stdout, // log INFO and above to stdout
      },
    ],
  });
}

method.setupLogFileStream = function setupLogFileStream(path, level) {
  _(this).mainLogger.addStream({
    path: path,
    level: level,
  });
};

method.errorHandler = function errorHandler(err, opts) {
  if (opts && opts['logger']) {
    opts['logger'].error(err);
  } else {
    _(this).mainLogger.error(err);
  }
};

method.isTrustedError = function isTrustedError(error) {
  return error.isOperational;
};

method.exitHandler = function exitHandler(error, reason) {
  if (error.message) error.message = reason + ' - ' + error.message;
  else error.message = reason;

  method.fatalHandler(error);
  if (!method.isTrustedError(error)) {
    process.nextTick(() => {
      process.exit(1);
    });
  }
};

method.fatalHandler = function fatalHandler(err) {
  // bonus: log the exception
  _(this).mainLogger.fatal(err);

  _(this).mainLogger.streams.forEach(function closeAllStreams(currStream) {
    console.log(currStream.stream.path);
    if (typeof currStream !== 'object') return;

    // throw the original exception once stream is closed
    currStream.stream.on('close', function(streamErr, stream) {
      throw err;
    });

    // close stream, flush buffer to disk
    currStream.stream.end();
  });
};

// module.exports = new ErrorHandler();
module.exports = ErrorHandler;
