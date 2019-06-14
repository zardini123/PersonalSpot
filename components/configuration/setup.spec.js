const path = require('path');
const fs = require('fs');
const util = require('util');

// Convert fs.readFile into Promise version of same
const readFile = util.promisify(fs.readFile);

describe('configuration', function() {
  describe('configuration setup', function() {
    it.skip('should create a basic config file in current directory', function() {});
  });
});
