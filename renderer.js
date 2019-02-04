// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const ipcRenderer = require('electron').ipcRenderer;

// document.querySelector('#btnEd').addEventListener('click', () => {
//   getData()
// })

const btnclick = document.getElementById('loadnewwindow');
btnclick.addEventListener('click', function () {
 //send the info to main process . we can pass any arguments as second param.
  ipcRenderer.send("btnclick"); // ipcRender.send will pass the information to main process
});
