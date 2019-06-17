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

function createConfigFile() {
  return new Promise(async (resolve, reject) => {
    const content = await readFile('./initalConfig.yml', 'utf-8');
    await writeFile(path.join(process.cwd(), 'config.yml'), content);
  });
}
