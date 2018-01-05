const admin = require('firebase-admin');
module.exports = function (req, res) {
    if (req.method === 'POST') {
        req.body.forEach(element => {
            if (element.sensorName === "cowo.door") {
                if (element.value === 1) {
                    admin.database().ref("/cowoDoor/state").set("open");
                }
                else {
                    admin.database().ref("/cowoDoor/state").set("closed");
                }
             }
        });
    }
    res.sendStatus(200);
}