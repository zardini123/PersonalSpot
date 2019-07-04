const bunyan = require('bunyan');

const method = Logger.prototype;

// TODO: Make file log rotating
const logger = bunyan.createLogger({
  name: 'PersonalSpot',
  streams: [
    {
      level: 'info',
      stream: process.stdout,
    },
  ],
});

/**
 * Setup logger to log at a certian path.
 * Will store log file at CWD if not specified.
 * @param       {String} logPath Path to store info log file.
 * @constructor
 */
function Logging(logPath) {
  if (logPath == null) logPath = process.cwd();

  logger.addStream({
    level: 'info',
    path: logPath,
  });
}

/**
 * Output info to logger.
 * This is used for informational purposes, not for logging of errors.
 * Use handleError() for errors.
 * @param  {String} info Information to log
 */
function handleInfo(info) {
  if (info instanceof Error) handleError(info);
  else {
    logger.info(info);
  }
}
method.handleInfo = handleInfo;

/**
 * Output error to logger.
 * @param  {Error} err error to log.
 */
function handleError(err) {
  logger.error(err);
}
method.handleError = handleError;

/**
 * Determines if error is operational
 * @param  {Error}  err [description]
 * @return {Boolean}     [description]
 */
function isTrustedError(err) {
  return err.isOperational;
}
method.isTrustedError = isTrustedError;

function exitHandler(error, reason) {
  if (error.message) error.message = reason + ' - ' + error.message;
  else error.message = reason;

  fatalHandler(error);
  if (!isTrustedError(error)) {
    process.nextTick(() => {
      process.exit(1);
    });
  }
}
method.exitHandler = exitHandler;

function fatalHandler(err) {
  // bonus: log the exception
  logger.fatal(err);

  logger.streams.forEach(function closeAllStreams(currStream) {
    if (typeof currStream !== 'object') return;

    // throw the original exception once stream is closed
    currStream.stream.on('close', function(streamErr, stream) {
      throw err;
    });

    // close stream, flush buffer to disk
    currStream.stream.end();
  });
}
method.fatalHandler = fatalHandler;

module.exports = Logger;
