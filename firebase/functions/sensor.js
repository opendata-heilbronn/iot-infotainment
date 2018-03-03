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
             if (element.sensorName === "emergency") {
                const emergencyState = {
                    lastChange : Date.now()
                };
                db.ref("/buttons/emergencyHistory").push().set(doorState);
             }
             if (element.sensorName === "off") {
                const emergencyState = {
                    lastChange : Date.now()
                };
                db.ref("/buttons/offHistory").push().set(doorState);
             }
             if (element.sensorName === "on") {
                const emergencyState = {
                    lastChange : Date.now()
                };
                db.ref("/buttons/onHistory").push().set(doorState);
             }
        });
    }
    res.sendStatus(200);
}
