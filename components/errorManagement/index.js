const ErrorHandler = require('./errorHandler.js');

// module.exports = errorHandler;
// module.exports.setupListeners = listeners;

module.exports = function(logFilePath) {
  module.exports = new ErrorHandler(logFilePath);

  return module.exports;
};
