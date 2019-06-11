// Require errorHandler to setup log path first
const errorManagement = require('..')('outTest.log');

// Setup listeners
const listeners = require('../listeners.js');
listeners();

// Trigger the uncaught exception
throw new Error('Woah there, Betsy');
