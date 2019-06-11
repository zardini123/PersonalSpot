const initalConfig = require('./initalConfig.js');

const chai = require('chai');
const should = chai.should();

describe('configuration', function() {
  describe('inital configuration JSON', function() {
    it('should return a JSON object', function() {
      const str = JSON.stringify(initalConfig());

      let isJson = false;
      try {
        JSON.parse(str);
        isJson = true;
      } catch (e) {
        console.log(e);
        isJson = false;
      }

      isJson.should.equal(true);
    });
  });
});
