// Modules to control application life and create native browser window
const electron = require('electron')
const {app, BrowserWindow} = electron

// const musicData = require('./src/musicData/importMusicData')
// musicData.setup(app.getPath('music'))
//
// app.quit()

const storage = require('electron-json-storage');
class Settings {
  constructor() {
    let val;
    storage.get('settings', (error, val) => {
      if (error) throw error;
    });

    this._data = val;
  }

  get data() {
    return this._data;
  }

  set data(newData) {
    storage.set('settings', newData, function(error) {
      if (error) throw error;
    });
    this._data = newData;
  }
}
var settings = new Settings()

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow

function createWindow () {
  // Create the browser window.
  // mainWindow = new BrowserWindow({width: 800, height: 600})
  const { width, height } = electron.screen.getPrimaryDisplay().workAreaSize

  let finalWidth = width * 0.8
  let size = {width: finalWidth, height: height}
  if (settings.data['size'] == null) {
    settings.data['size'] = size
  } else {
    size = settings.data['size']
  }

  mainWindow = new BrowserWindow({width: size['width'], height: size['height']})

  // and load the index.html of the app.
  mainWindow.loadFile('./src/index.html')

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

// const storage = require('electron-json-storage');
const sqlite3 = require('sqlite3').verbose();
const path = require('path');
const fs = require('fs');

function setup() {
  if (settings.data == null) settings.data = {};

  createWindow();
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', setup)

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
