const path = require('path');
const fs = require('fs');

const sqlite = require('better-sqlite3');

// const {promisify} = require('util');
// const access = promisify(fs.access);

const config = require(path.join('..', 'config'));

function setupProcedure() {
  return new Promise(async (resolve, reject) => {
    try {
      // Get repo directory from enviroment
      const repoDir = config.getRepoDirectory();
      // Get or create SQLite3 database
      const dbPath = path.join(repoDir, 'main.db');
      const db = sqlite(dbPath);
      // Create info table if doesnt exist
      let stmt = db.prepare(
          'CREATE TABLE IF NOT EXISTS meta_info (' +
          'key TEXT,' +
          'value TEXT,' +
          'PRIMARY KEY(key)' +
          ');'
      );
      let info = stmt.run();
      // Check if repo version number exists
      stmt = db.prepare('SELECT key, value FROM meta_info WHERE key=?;');
      const out = stmt.get('database_version');
      let dbVersion = '';
      if (out == null) {
        stmt = db.prepare('INSERT INTO meta_info (key, value) VALUES(?, ?)');
        // TODO: Get database version ID from the "database" component
        dbVersion = '0.0.1';
        info = stmt.run('database_version', dbVersion);
      } else {
        dbVersion = out.value;
      }
      console.log(dbVersion);
      // If exists:
      //    Get repo version number
      // If not:
      //    Create info table
      //    Set database version number to most recent database version
      //        (this logic will probably be in another module)
      // Check and apply database schema
      //  (schema: database with fixed file structure)
      //    Add new folders directly that are part of the spec
      //    Move content or rename folders based on newest spec
      //    Cleanup folders that are unused in newest spec
      //    Log and shutdown if any failures
      // Start server (continue app.js sequence)
      resolve();
    } catch (err) {
      reject(err);
    }
  });
}
module.exports.setupProcedure = setupProcedure;
