const express = require('express');
const app = express();
const port = 3000;

function getRandomInt(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

const path = require('path');

app.use(express.static('public'));

// How the webpage works:
//  1) User connects at endpoint '/' by entering http://localhost:3000
//      (http://localhost:3000/ would do the same thing)
//  2) Once connected to endpoint '/', the backend (node.js server) will send
//      a html file.  Browser interprets and displays the html.
//  3) At the beginning of the html in the head, the jQuery library is included
//      via Google's CDN, and frontEnd.js is loaded.
//  4) Upon loading, frontEnd.js will send a GET AJAX request back to the
//      backend for the data under the '/images' endpoint.  The backend will
//      then send to the frontend an array of image urls.
//  5) frontEnd.js will then take all the image urls and make appropriate divs
//      and imgs for displaying the images.
//  This idea can be thought of as "client-side" or "frontend" rendering, as
//  only the info is provided by the backend, and the frontend interprets the
//  info and creates the appropriate webpage using that data.  (Another phrase
//  to describe it is a "dynamic" [not static] webpage)
app.get('/', (req, res) => {
  return res.sendFile(path.join(__dirname, 'index.html'));
});

// TODO: All the image links should not be sent at once.  It should be sent
//  probably based around the user's scrolled location (Some JSON packets
//  could possibly convey that info?)
app.get('/images', (req, res) => {
  const arr = [];
  // Small image test.  This image should be scaled to the size of the container
  // FIXME:  Images smaller than container does not scale
  arr.push('https://via.placeholder.com/50x50.png');
  for (let i = 0; i < 100; i++) {
    const width = getRandomInt(50, 1000);
    const height = getRandomInt(50, 1000);
    arr.push('https://via.placeholder.com/' + width + 'x' + height + '.png');
  }
  return res.send(arr);
});

app.listen(port, () => console.log(`Example app listening on port ${port}!`));
