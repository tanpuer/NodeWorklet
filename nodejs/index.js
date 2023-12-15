const fetch = require('node-fetch');
const addon = require("./build/Debug/worklet.node");
let result = addon.init();

console.log("addon init =", result);
console.log(global._WORKLET_RUNTIME);
console.log(global._WORKLET);
console.log(global._REANIMATED_VERSION_CPP);
console.log(global._REANIMATED_IS_REDUCED_MOTION);
console.log(global._IS_FABRIC);

const url = 'http://localhost:8080/index.bundle.js'
fetch(url)
    .then(response => {
        if (response.ok) {
            return response.text();
        }
        throw new Error('Network response was not ok.');
    })
    .then(text => {
        addon.eval(text, url);
    })
    .catch(error => {
        console.error('Fetch error:', error);
    });

