const yaml = require('js-yaml');
const fs = require('fs');

const chai = require('chai');
const should = chai.should();

const setup = require('./setup.js');

describe('configuration', function() {
  describe('inital configuration file', function() {
    it('should return a valid YAML object', function() {
      // Get document, or throw exception on error
      try {
        const file = fs.readFileSync(
            './components/configuration/initalConfig.yml',
            'utf8'
        );
        yaml.safeLoad(file);
      } catch (e) {
        should.fail(e);
      }
    });
  });
  describe('configuration setup', function() {
    it('should create inital config file in current directory', function() {
      setup
          .createConfigFile()
          .then(() => {
            throw new Error('owo');
          })
          .catch((err) => {
            should.fail(err);
          });
    });
  });
});
