// const assert = require('assert');
// describe('Array', function() {
//   describe('#indexOf()', function() {
//     it('should return -1 when the value is not present', function() {
//       assert.equal([1, 2, 3].indexOf(4), -1);
//     });
//   });
// });

const childProcess = require('child_process');
const path = require('path');

const chai = require('chai');
const expect = chai.expect;

describe('listeners', function() {
  describe('uncaughtException', function() {
    it.skip('correctly handles an uncaught exception', function(done) {
      const proc = childProcess.fork(
          path.resolve(__dirname, './listeners.spec/uncaughtExceptionCase.js'),
          {
            env: process.env,
            silent: false,
          }
      );

      let output = '';

      if (proc.silent) {
        proc.stdout.on('data', function(data) {
          output += data.toString();
        });

        proc.stderr.on('data', function(data) {
          output += data.toString();
        });
      }

      proc.on('exit', function(code) {
        expect(code).to.equal(1);

        // FIXME: BAD BAD BAD
        expect(output).to.contain('uncaughtException');

        // const outData = fs.readFileSync('./out.log');
        // expect(outData).to.contain('uncaughtException');
        done();
      });
    });
  });
});
