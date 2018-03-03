const admin = require('firebase-admin');
const db = admin.database();
const moment = require('moment');

module.exports = function (req, res) {
    if (req.method === 'POST') {
        req.body.forEach(element => {
            if (element.sensorName === "cowo.door") {
                const doorState = {
                    lastChange: Date.now()
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
            if (element.sensorName === "cowo.emergency") {
                if (element.value === "emergency") {
                    const emergencyState = {
                        lastChange: Date.now()
                    };
                    db.ref("/buttons/emergencyHistory").push().set(emergencyState);
                }
                if (element.value === "off") {
                    const offState = {
                        lastChange: Date.now()
                    };
                    db.ref("/buttons/offHistory").push().set(offState);
                }
                if (element.value === "on") {
                    const onState = {
                        lastChange: Date.now()
                    };
                    db.ref("/buttons/onHistory").push().set(onState);
                }
            }
        });
    }
    res.sendStatus(200);
}
