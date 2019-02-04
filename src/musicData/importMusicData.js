// Unable to link native dependencies/C++ modules to production
// https://github.com/electron-userland/electron-builder/issues/3455

const path = require('path')
const {app} = require('electron');

const process = require('process')

var appPath = app.getAppPath();

libraryPath = path.join(process.resourcesPath, "Libraries")
if (!app.isPackaged) {
  libraryPath = path.join(appPath, "libraries/lib")
}
originalCwd = process.cwd()

//console.log(`Starting directory: ${originalCwd}`);
try {
  process.chdir(libraryPath);
  //console.log(`New directory: ${process.cwd()}`);
} catch (err) {
  console.error(`chdir: ${err}`);
  return null;
}

var binary = require('node-pre-gyp');
var binding_path = binary.find(path.resolve(path.join(appPath,'package.json')));
const musicData = require(binding_path);

//console.log(`Starting directory: ${process.cwd()}`);
try {
  process.chdir(originalCwd);
  //console.log(`New directory: ${process.cwd()}`);
} catch (err) {
  console.error(`chdir: ${err}`);
  return null;
}

module.exports = musicData;
