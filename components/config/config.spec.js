const chai = require('chai');
const expect = chai.expect;

const tmp = require('tmp');
const path = require('path');
const fs = require('fs-extra');

const config = require('../config');

describe('config', function() {
  describe('verifyRepoDir()', function() {
    it('should fail on bad REPO_DIR', function(done) {
      process.env.REPO_DIR = 'asdoifj///asdo\fi4uhas5!dsajhdf';
      config
          .verifyRepoDir()
          .then(() => {
            done(
                new Error(
                    'verifyRepoDir() should not succeed with path ' +
                process.env.REPO_DIR
                )
            );
          })
          .catch((err) => {
            done();
          });
    });
    it('shoud succeed on fine REPO_DIR', function(done) {
      process.env.REPO_DIR = 'aba/b/c';
      config
          .verifyRepoDir()
          .then(() => {
            done();
          })
          .catch((err) => {
            done(err);
          });
    });
  });
  describe('getter functions', function() {
    describe('getRepoDirectory()', function() {
      before(function() {
        tempDir = tmp.dirSync({unsafeCleanup: true});
      });
      it('should create and return the full REPO_DIR directory without errors', function(done) {
        process.env.REPO_DIR = path.join(tempDir.name, 'test', 'repo');

        config
            .getRepoDirectory()
            .then((dir) => {
              expect(dir).to.equal(process.env.REPO_DIR);
              if (!fs.pathExistsSync(dir)) {
                done(new Error('Path was not created!'));
              }
              done();
            })
            .catch((err) => {
              done(err);
            });
      });
      after(function() {
        // Manual cleanup of temporary directory
        tempDir.removeCallback();
      });
    });
    describe('getPort()', function() {
      it('expect to return an number object', function() {
        process.env.PORT = 3000;

        expect(config.getPort()).to.be.a('number');
      });
    });
  });
});
