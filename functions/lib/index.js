"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.helloWorld = void 0;
const functions = require("firebase-functions");
// Start writing functions
// https://firebase.google.com/docs/functions/typescript
exports.helloWorld = functions.https.onRequest((request, response) => {
    console.log("Test");
});
//# sourceMappingURL=index.js.map