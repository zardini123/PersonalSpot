const errorHandler = require('../errorManagement');

module.exports = function setupListeners() {
  process.on('uncaughtException', function(err) {
    // prevent infinite recursion
    process.removeListener('uncaughtException', arguments.callee);

    errorHandler.exitHandler(err, 'uncaughtException');
  });

  process.on('uncaughtRejection', function(err) {
    errorHandler.exitHandler(err, 'uncaughtRejection');
  });
};

// process.on('beforeExit', () => handler('beforeExit'));
// process.on('exit', () => handler('exit'));
// process.on('SIGINT', () => handler('SIGINT'));
// process.on('SIGQUIT', () => handler('SIGQUIT'));
// process.on('SIGTERM', () => handler('SIGTERM'));
