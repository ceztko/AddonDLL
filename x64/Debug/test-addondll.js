const { dlopen } = require('node:process');

console.log('Try load addondll.dll ..')

const mod = { exports: {} };
process.dlopen(mod, '.\\addondll.dll');
var addon = mod.exports

console.log('This should be eight:', addon.add(3, 5))
