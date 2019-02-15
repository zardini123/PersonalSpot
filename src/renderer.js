// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const { remote } = require('electron')

var colorState = false;

const btnclick = document.getElementById('loadnewwindow');
btnclick.addEventListener('click', function () {
 //send the info to main process . we can pass any arguments as second param.
 colorState = !colorState;

  if (colorState)
    window.localStorage.os_theme = 'dark'
  else
    window.localStorage.os_theme = 'light'

  if ('__setTheme' in window) {
    window.__setTheme()
  }
});

if (process.platform == 'darwin') {
  const { systemPreferences } = remote

  const setOSTheme = () => {
    let theme = systemPreferences.isDarkMode() ? 'dark' : 'light'
    window.localStorage.os_theme = theme

    //
    // Defined in index.html, so undefined when launching the app.
    // Will be defined for `systemPreferences.subscribeNotification` callback.
    //
    if ('__setTheme' in window) {
      window.__setTheme()
    }
  }

  systemPreferences.subscribeNotification(
    'AppleInterfaceThemeChangedNotification',
    setOSTheme,
  )

  setOSTheme()
}

const sqlite3 = require('sqlite3').verbose();

window.onload = function() {
  let db = new sqlite3.Database('/Users/Tarasik/Music/Apollo/apollo.db', (err) => {
    if (err) {
      console.error(err.message);
    }
    console.log('Connected to the database.');
  });

  var div = document.getElementById("images");
  div.innerHTML = ""; // clear images

  var sql = 'SELECT hash, base64, codec_ID FROM album_art';
  db.all(sql, [], (err, rows) => {
    if (err) {
      throw err;
    }
    rows.forEach((row) => {
      var imagem = document.createElement("img");
      imagem.src = "data:image;base64," + row['base64'];
      imagem.setAttribute("id", "album_art");
      div.appendChild(imagem);
    });
  });

  db.close((err) => {
    if (err) {
      return console.error(err.message);
    }
    console.log('Close the database connection.');
  });
}

// function placeImage(x) {
//     var div = document.getElementById("images");
//
//     div.innerHTML = ""; // clear images
//
//     for (counter=1;counter<=x;counter++) {
//         var imagem=document.createElement("img");
//         imagem.src="borboleta/Borboleta"+counter+".png";
//         div.appendChild(imagem);
//     }
// }
//
// window.onload = function() {
//     placeImage(48);
// };
