const addon = require("./build/Debug/worklet.node");
let result = addon.init();

console.log("addon init =", result);
console.log(global._WORKLET_RUNTIME);
console.log(global._WORKLET);
console.log(global._REANIMATED_VERSION_CPP);
console.log(global._REANIMATED_IS_REDUCED_MOTION);
console.log(global._IS_FABRIC);

