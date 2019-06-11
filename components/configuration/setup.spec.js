const path = require('path');
const fs = require('fs');

describe('configuration', function() {
  describe('configuration setup', function() {
    it('should create a basic config file in current directory', function() {
      fs.readFile(path.join(process.cwd(), 'config.json'), (err, data) => {
        if (err) throw err;
        const student = JSON.parse(data);
      });
    });
  });
});
