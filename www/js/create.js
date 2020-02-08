const urlParams = new URLSearchParams(window.location.search);
const myParam   = urlParams.get('grid');
console.log("my param: " + myParam);
