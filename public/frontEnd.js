const upperBound = 192;
const lowerBound = 158;
const margin = 22;

const root = document.documentElement;

function roundUp(numToRound, multiple) {
  if (multiple == 0) return numToRound;

  const remainder = numToRound % multiple;
  if (remainder == 0) return numToRound;

  return numToRound + multiple - remainder;
}

function setCoverArtVars() {
  // console.log(window.innerWidth);
  const targetWidth = roundUp(
      document.getElementsByClassName('contentViewport')[0].offsetWidth,
      8
  );
  // const targetWidth = window.innerWidth;
  const divisor = Math.floor(targetWidth / (lowerBound + margin));
  console.log(divisor);
  const containerSize = targetWidth / divisor - margin;
  // console.log(targetWidth);
  root.style.setProperty('--cover-container-size', containerSize + 'px');

  // const marginSize = targetWidth / (containerSize * (divisor - 1));
  // console.log(marginSize);
  // root.style.setProperty('--cover-container-margin', 0);
}

window.addEventListener('DOMContentLoaded', () => setCoverArtVars());
window.addEventListener('resize', () => setCoverArtVars());

const config = {
  rootMargin: '400px 0px 400px 0px',
  threshold: 0,
};

// function preloadImage(parent) {
//   const img = parent.getElementsByTagName('img')[0];
//   const src = img.getAttribute('data-src');
//   if (!src) {
//     return;
//   }
//   img.src = src;
// }
//
// const observer = new IntersectionObserver(function(entries, self) {
//   entries.forEach((entry) => {
//     if (entry.isIntersecting) {
//       // console.log(`Image ${entry.target.src} is in the viewport!`);
//       preloadImage(entry.target);
//       // Stop watching and load the image
//       self.unobserve(entry.target);
//     }
//   });
// }, config);

$.ajax({
  url: 'images',
  type: 'GET',
  success: function(result) {
    // REVIEW: Should the container element be found via a class, or an id?
    const parent = document.getElementsByClassName('contentViewport')[0];

    // TODO: Only the div's that are in and close to the viewport should be
    // created and rendered.  Other images and data outside the viewport should
    // be cached.
    for (i = 0; i < result.length; i++) {
      const div = document.createElement('div');
      // div.id = i;
      div.className = 'imageContainer';
      div.innerHTML = '<img src="' + result[i] + '" class = "image"/>';
      // observer.observe(div);
      parent.appendChild(div);
    }
  },
  error: function(error) {
    console.error(error);
  },
});
