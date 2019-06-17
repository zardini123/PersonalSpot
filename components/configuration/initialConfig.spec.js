const JSON5 = require('json5');

const initalConfig = require('./initalConfig.js');

const chai = require('chai');
const should = chai.should();

describe('configuration', function() {
  describe('inital configuration JSON', function() {
    it.skip('should return a JSON object', function() {
      const str = JSON5.stringify(initalConfig());

      let isJson = false;
      try {
        JSON5.parse(str);
        isJson = true;
      } catch (e) {
        console.log(e);
        isJson = false;
      }

      isJson.should.equal(true);
    });
  });
});
