const yaml = require('js-yaml');
const fs = require('fs');

const chai = require('chai');
const should = chai.should();

const tmp = require('tmp');

const setup = require('./setup.js');

describe('configuration', function() {
  describe('setup', function() {
    // describe.skip('validateConfigFile()', function() {
    //   it('expect to fail upon verifying a non-YAML file', function(done) {
    //     try {
    //       setup.validateConfigFile('app.js');
    //       done(
    //           new Error(
    //               'validateConfigFile() did not throw an error upon ' +
    //             'reading app.js'
    //           )
    //       );
    //     } catch (err) {
    //       done();
    //     }
    //   });
    //   it('expect to succeed upon verifying a valid YAML file', function(done) {
    //     try {
    //       setup.validateConfigFile('testConfig.yml');
    //       done();
    //     } catch (err) {
    //       done(err);
    //     }
    //   });
    // });

    describe('setupProcedure()', function() {
      before(function() {
        tempDir = tmp.dirSync({unsafeCleanup: true});

        process.env.REPO_DIR = tempDir.name;
      });

      describe('throws correct error in the event of...', function() {
        it.skip('no config file', function(done) {});
        it.skip('config file exists, but is invalid', function() {});
        it.skip('invalid path for the repository', function() {});
        it.skip('creates proper repository structure', function() {});
        it('function should not throw any uncaught errors', function(done) {
          setup
              .setupProcedure()
              .then(() => done())
              .catch((err) => done(err));
        });
      });
      after(function() {
        // Manual cleanup of temporary directory
        tempDir.removeCallback();
      });
    });
  });
});
