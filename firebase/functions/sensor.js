const admin = require('firebase-admin');
const db = admin.database();
const moment = require('momentjs');

module.exports = function (req, res) {
    if (req.method === 'POST') {
        req.body.forEach(element => {
            if (element.sensorName === "cowo.door") {
                const doorState = {
                    lastChange : Date.now()
                };
                if (element.value === 1) {
                    doorState.state = "open";
                }
                else {
                    doorState.state = "closed";
                }
                db.ref("/cowoDoor").set(doorState);
                db.ref("/doorHistory").push().set(doorState);
             }
        });
    }
    res.sendStatus(200);
}