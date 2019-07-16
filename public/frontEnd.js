$.ajax({
  url: 'images',
  type: 'GET',
  success: function(result) {
    // REVIEW: Should the container element be found via a class, or an id?
    const parent = document.getElementsByClassName('images')[0];

    // TODO: Only the div's that are in and close to the viewport should be
    // created and rendered.  Other images and data outside the viewport should
    // be cached.
    for (i = 0; i < result.length; i++) {
      const div = document.createElement('div');
      div.id = i;
      div.className = 'imageContainer';
      div.innerHTML = '<img src="' + result[i] + '" class = "image"/>';
      parent.appendChild(div);
    }
  },
  error: function(error) {
    console.error(error);
  },
});
