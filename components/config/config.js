const validatePath = require('valid-path');

// FIXME: getRepoDirectory() uses blocking functions.  Can potentially cause
//  delays in code execution upon calling the function often
function getRepoDirectory() {
  const validPath = validatePath(process.env.REPO_DIR);
  if (validPath === true) {
    return process.env.REPO_DIR;
  } else {
    throw new Error(validPath);
  }
}
module.exports.getRepoDirectory = getRepoDirectory;

// FIXME: getPort() uses blocking functions.  Can potentially cause
//  delays in code execution upon calling the function often
function getPort() {
  return parseInt(process.env.PORT, 10);
}
module.exports.getPort = getPort;
