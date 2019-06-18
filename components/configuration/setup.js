const yaml = require('js-yaml');

const path = require('path');
const fs = require('fs');

const {promisify} = require('util');
const writeFile = promisify(fs.writeFile);
const readFile = promisify(fs.readFile);

function checkIfSetup() {
  // Check if config file exists
  // If file does not exist:
  //    Create basic config file
  //    Return "please setup config"
  //    Shutdown
  // If file does exist:
  //    Check if valid yaml
  //    Check for valid repo path
  //    Check if repository directories exist
  //    If not:
  //      Setup repository directories
  //        - Media
  //        - cover_art
  //    Check if logging directories exist
  //    If not:
  //      Setup logging directories
  //    Check if SQLite database exist:
  //    If exists:
  //      Get version number
  //    If not:
  //      Create SQLite3 database
  //      Set database version number
  //    Log and shutdown if any failures
  // Start server (continue app.js sequence)
}

module.exports.validateConfigFile = function validateConfigFile(name) {
  if (!name) name = 'config.yml';
  yaml.safeLoad(fs.readFile(path.join(process.cwd(), name)));
};

module.exports.createConfigFile = function createConfigFile(name) {
  return new Promise(async (resolve, reject) => {
    try {
      if (!name) name = 'config.yml';

      const content = await readFile(
          path.join(__dirname, 'initalConfig.yml'),
          'utf-8'
      );
      // Have a call of safeLoad here to ensure the contents of the inital
      //    configuration hasn't been tampered with too bad.
      yaml.safeLoad(content);
      await writeFile(path.join(process.cwd(), name), content);
      await readFile(path.join(process.cwd(), name));
      resolve();
    } catch (err) {
      reject(err);
    }
  });
};
