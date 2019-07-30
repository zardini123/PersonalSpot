const albumContainers = [];
let contentViewport;

const upperBound = 192;
const lowerBound = 158;
const startMargin = 22;
let margin = startMargin;

const textSize = 10;
const startTopMargin = textSize * 4;
let topMargin = startTopMargin;

const inlineImgSize = 50;

const root = document.documentElement;

const listView = 0;
const inlineCoverAndTitleView = 1000;
const coverAndTitleView = 2000;
const coverView = 3000;

let sliderVal = 2000;

function clamp(value, min, max) {
  return Math.min(Math.max(value, min), max);
}

function sliderInput(val) {
  sliderVal = val;
  transitions(sliderVal);
}

function calculateWidth(element) {
  const rect = element.getBoundingClientRect();

  let width;
  if (rect.width) {
    // `width` is available for IE9+
    width = rect.width;
  } else {
    // Calculate width for IE8 and below
    width = rect.right - rect.left;
  }

  return width;
}

function transitions(val) {
  // FIXME:  Transitions don't fully happen if user jumps between two
  //  "sections."
  if (val >= listView && val < inlineCoverAndTitleView) {
  } else if (val >= inlineCoverAndTitleView && val < coverAndTitleView) {
    const diff = coverAndTitleView - inlineCoverAndTitleView;
    const val2 = (val - inlineCoverAndTitleView) / diff;
    coverAndTitleViewTransition(val2);
  } else if (val >= coverAndTitleView && val <= coverView) {
    const diff = coverView - coverAndTitleView;
    const val2 = (val - coverAndTitleView) / diff;
    coverViewTransition(val2);
  }
}
function coverAndTitleViewTransition(val) {
  const inv = 1 - val;
  margin = val * startMargin;
  // const targetWidth = val *
  const bottomHalf = clamp((val - 0.25) / 0.75, 0, 1);
  const topHalf = clamp((inv - 0.25) / 0.75, 0, 1);
  const percent = topHalf * 100;
  const px = bottomHalf * lowerBound + margin;
  root.style.setProperty(
      '--target-entry-width',
      'calc(' + px + 'px + ' + percent + '%' + ')'
      // inv * 100 + '%'
  );

  const finalWidth = lowerBound * val + inlineImgSize * inv;
  root.style.setProperty('--cover-size', finalWidth + 'px');

  root.style.setProperty('--margin', margin + 'px');
  // setCoverArtVars();
}
function coverViewTransition(val) {
  const inv = 1 - val;
  topMargin = inv * startTopMargin;
  margin = inv * startMargin;
  // Fade h2 fully in first half-range
  root.style.setProperty('--h2-opacity', clamp((inv - 0.5) * 2, 0, 1));
  // Fade h1 fully in second half-range
  const halfTwo = clamp(inv / 0.5, 0, 1);
  root.style.setProperty('--h1-opacity', halfTwo);
  root.style.setProperty('--line-height', halfTwo * 1.5 + 'em');

  const width = calculateWidth(albumContainers[0]);
  root.style.setProperty('--cover-size', width - margin + 'px');

  root.style.setProperty('--target-entry-width', lowerBound + margin + 'px');

  root.style.setProperty('--margin', margin + 'px');
  // root.style.setProperty('--entry-width', width + 'px');
  // setCoverArtVars();
}

// window.addEventListener('DOMContentLoaded', () => createChunk());
window.addEventListener('resize', () => transitions(sliderVal));

function sliderChange(val) {
  console.log('changed: ' + val);
}

function random_bg_color() {
  const x = Math.floor(Math.random() * 256);
  const y = Math.floor(Math.random() * 256);
  const z = Math.floor(Math.random() * 256);
  const bgColor = 'rgb(' + x + ',' + y + ',' + z + ')';

  return bgColor;
}

function createChunk() {
  contentViewport = document.getElementsByClassName('contentViewport')[0];

  const chunk = document.createElement('div');
  chunk.className = 'chunk';
  chunk.style.background = random_bg_color();

  contentViewport.appendChild(chunk);
  // observer.observe(chunk);
}

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
      const container = document.createElement('div');
      container.className = 'entry';

      const div2 = document.createElement('div');
      div2.className = 'imageSizer';

      const div = document.createElement('div');
      // div.id = i;
      div.className = 'imageContainer';
      div.innerHTML =
        '<img src="' + result[i]['coverURL'] + '" class = "image"/>';
      // observer.observe(div);

      div2.appendChild(div);
      container.appendChild(div2);
      container.innerHTML +=
        '<h1>' +
        result[i]['albumName'] +
        '</h1><h2>' +
        result[i]['artistName'] +
        '</h2>';

      parent.appendChild(container);
      albumContainers.push(container);
    }
    setCoverArtVars();
  },
  error: function(error) {
    console.error(error);
  },
});

// Some old IntersectionObserver code, for later

// const config = {
//   rootMargin: '400px 0px 400px 0px',
//   threshold: 0,
// };
// function preloadImage(parent) {
//   const img = parent.getElementsByTagName('img')[0];
//   const src = img.getAttribute('data-src');
//   if (!src) {
//     return;
//   }
//   img.src = src;
// }
//
//
// const observer = new IntersectionObserver(function(entries, self) {
//   entries.forEach((entry) => {
//     if (entry.isIntersecting) {
//       // console.log(`Image ${entry.target.src} is in the viewport!`);
//       // preloadImage(entry.target);
//       // Stop watching and load the image
//       createChunk();
//       self.unobserve(entry.target);
//     }
//   });
// }, config);

// Old code!!!
// Kept in need for future functions
function roundUp(numToRound, multiple) {
  if (multiple == 0) return numToRound;

  const remainder = numToRound % multiple;
  if (remainder == 0) return numToRound;

  return numToRound + multiple - remainder;
}

function setCoverArtVars() {
  return;

  contentViewport = document.getElementsByClassName('contentViewport')[0];

  const targetWidth = roundUp(contentViewport.offsetWidth, 8);

  let divisor = Math.floor(targetWidth / (lowerBound + margin));
  let coverSize = targetWidth / divisor - margin;
  if (val != undefined) {
    const tempDivisor = divisor;
    const inv = 1 - val;
    divisor = Math.floor(val * tempDivisor + inv);

    const tmpContSize = coverSize;
    coverSize = tmpContSize * val + inlineImgSize * inv;

    margin = targetWidth / divisor - coverSize;
    root.style.setProperty('--entry-width', margin + coverSize + 'px');
  } else {
    root.style.setProperty('--entry-width', 'var(--cover-size)');
  }

  root.style.setProperty('--cover-size', coverSize + 'px');

  for (let i = 0; i < albumContainers.length; i++) {
    const level = Math.floor(i / divisor);
    albumContainers[i].style.top =
      'calc(' + coverSize * level + 'px + ' + level * topMargin + 'pt)';

    const multiple = i % divisor;
    const mainStyle = multiple * coverSize + multiple * margin;
    albumContainers[i].style.left = mainStyle + 'px';
  }
}
