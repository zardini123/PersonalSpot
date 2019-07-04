const fs = require('fs-extra');
const path = require('path');

const config = require(path.join('..', 'config'));

function getSystemDir() {
  const pth = path.join(config.getRepoDirectory(), 'system');
  return ensureDir(pth);
}

function getLogDir() {
  const pth = path.join(getSystemDir(), 'log');
  return ensureDir(pth);
}
module.exports.getLogDir = getLogDir();
