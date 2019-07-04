const validatePath = require('valid-path');

const fs = require('fs-extra');

// REVIEW: verifyRepoDir() uses blocking functions.  Can potentially cause
//  delays in code execution upon calling the function often
/**
 * Checks if env.REPO_DIR is a valid path.
 * @return {String} Resolves with env.REPO_DIR on valid,
 * rejects with Error object if invalid.
 */
function verifyRepoDir() {
  return new Promise(async (resolve, reject) => {
    const validPath = validatePath(process.env.REPO_DIR);
    if (validPath === true) {
      resolve(process.env.REPO_DIR);
    } else {
      reject(new Error(validPath));
    }
  });
}
module.exports.verifyRepoDir = verifyRepoDir;

/**
 * Returns the repo directory stored in env (REPO_DIR).
 * Checks to make sure dir is a valid dir string, then it ensures the dir
 * exists.
 * @return {String} Resolves with env.REPO_DIR on valid,
 * rejects with Error object if invalid.
 */
function getRepoDirectory() {
  return new Promise(async (resolve, reject) => {
    try {
      const path = await verifyRepoDir();
      await fs.ensureDir(path);
      await resolve(path);
    } catch (err) {
      reject(err);
    }
  });
}
module.exports.getRepoDirectory = getRepoDirectory;

// function getBasePath() {}

// REVIEW: getPort() uses blocking functions.  Can potentially cause
//  delays in code execution upon calling the function often
/**
 * Returns the server port stored in env (PORT).
 * Parses port to an integer object.
 * @return {Int} Parsed port.
 */
function getPort() {
  return parseInt(process.env.PORT, 10);
}
module.exports.getPort = getPort;
