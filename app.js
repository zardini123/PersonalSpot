const express = require('express');
const helmet = require('helmet');

const path = require('path');
const config = require(path.resolve('.', 'components', 'config'));

const app = express();
app.use(helmet());

app.get('/', function(req, res) {
  res.sendFile('index.html', {root: __dirname});
});

const port = config.getPort();
app.listen(port, () => console.log(`Example app listening on port ${port}!`));
