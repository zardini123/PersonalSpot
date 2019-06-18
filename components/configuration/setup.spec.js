const yaml = require('js-yaml');
const fs = require('fs');

const chai = require('chai');
const should = chai.should();
const expect = chai.expect();

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
    it('should create inital config file in current directory', function(done) {
      setup
          .createConfigFile('testConfig.yml')
          .then(() => {
            done();
          })
          .catch((err) => {
            should.fail(err);
          });
    });
    describe('validateConfigFile()', function() {
      it.skip('expect to fail upon verifying a non-YAML file', function() {
        // expect(setup.validateConfigFile('package.json')).to.fail;
      });
      it.skip('should determine created config file is a valid YAML file', function(done) {});
    });
  });
});
