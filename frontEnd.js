var imageNames = [
  "freeX.jpeg",
  "questionMark.png",
  "revenge.jpg",
  "seventeen.jpg",
  "tallPic.jpg",
  "widePic.jpg"
];
for (i = 0; i < imageNames.length; i++) {
  var div = document.createElement("div");
  div.id = i;
  div.className = "imageContainer";
  div.innerHTML = '<img src="./temp/' + imageNames[i] + '" class = "image"/>';
  document.body.appendChild(div);
}

/*document.getElementById("image1").innerHTML =
  '<img src="./temp/seventeen.jpg" alt="Free X"/>';
var div = document.createElement("div2");
document.body.appendChild(div);*/
