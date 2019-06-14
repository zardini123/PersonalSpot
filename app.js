const express = require('express');
const helmet = require('helmet');

const app = express();
app.use(helmet());

const port = 3000;

const errorManagement = require('./components/errorManagement')('./out.log');
require('./components/listeners')();

const bunyan = require('bunyan');

throw new Error();

app.get('/', function(req, res) {
  res.sendFile('index.html', {root: __dirname});
});

app.listen(port, () => console.log(`Example app listening on port ${port}!`));
