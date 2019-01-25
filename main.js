// Modules to control application life and create native browser window
const {app, BrowserWindow} = require('electron')

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow

function createWindow () {
  // Create the browser window.
  mainWindow = new BrowserWindow({width: 800, height: 600})

  // and load the index.html of the app.
  mainWindow.loadFile('index.html')

  // Open the DevTools.
  mainWindow.webContents.openDevTools()

  // Emitted when the window is closed.
  mainWindow.on('closed', function () {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null
  })
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow)

// Quit when all windows are closed.
app.on('window-all-closed', function () {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit()
  }
})

app.on('activate', function () {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    createWindow()
  }
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.

// Scanning of user's library should eventually be done in a child process.
// This is to avoid the main and render threads being cloged with other
// bulky aquisition processes.
// https://medium.freecodecamp.org/node-js-child-processes-everything-you-need-to-know-e69498fe970a
const musicLibraryURL = "/Users/Tarasik/Music/iTunes/iTunes Media/Music"

// Unable to link native dependencies/C++ modules to production
// https://github.com/electron-userland/electron-builder/issues/3455

const path = require('path')

const process = require('process')

console.log(process.resourcesPath)
console.log(app.isPackaged)

libraryPath = path.join(process.resourcesPath, "Libraries")
if (!app.isPackaged) {
  libraryPath = path.join(app.getAppPath(), "libraries/lib")
}
originalCwd = process.cwd()

console.log(`Starting directory: ${originalCwd}`);
try {
  process.chdir(libraryPath);
  console.log(`New directory: ${process.cwd()}`);
} catch (err) {
  console.error(`chdir: ${err}`);
}

var binary = require('node-pre-gyp');
var binding_path = binary.find(path.resolve(path.join(__dirname,'./package.json')));
var musicData = require(binding_path);

console.log(`Starting directory: ${process.cwd()}`);
try {
  process.chdir(originalCwd);
  console.log(`New directory: ${process.cwd()}`);
} catch (err) {
  console.error(`chdir: ${err}`);
}

musicData.listFiles(musicLibraryURL)

/*
walker.on("file", function (root, fileStats, next) {
  var fullPath = path.join(root, fileStats.name)
  console.log(fullPath)
  console.log(musicData.getMetadata(fullPath))
  next();
});
*/
