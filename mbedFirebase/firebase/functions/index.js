const functions = require('firebase-functions');    // import Cloud Functions for Firebase SDK 
const admin = require('firebase-admin'); // import Firebase Admin SDK
admin.initializeApp();

exports.timeStamp = functions.database.ref('/test/v1/{pushId}/name').onCreate((snapshot, context) => {
    const name = snapshot.val();
    const key = snapshot.ref.parent.key;
    var now = new Date();
    console.log('Timestamp of ' + name + ':' + now.toLocaleDateString() + ', ' + now.toLocaleTimeString());
    admin.database().ref('/latest').set({name: name, key: key});
    return snapshot.ref.parent.child('timestamp').set(now.getTime());
});

exports.queryBefore = functions.https.onRequest(async (req, res) => {
    const mins  = req.query.mins;
    console.log('Param: ' + mins);
    var tprev = new Date().getTime();
    tprev = tprev - (mins*60*1000);
    var listKeys = [];
    var query = admin.database().ref('/test/v1').orderByChild('timestamp').startAt(tprev);      
    await query.once('value').then((snapshots) => {
        snapshots.forEach((snapshot) => {
            listKeys.push(snapshot.key);
            console.log(snapshot.val());
        });
    });
    var json_resp = {keys: listKeys}
    res.send(json_resp);
});

// // Create and Deploy Your First Cloud Functions
// // https://firebase.google.com/docs/functions/write-firebase-functions
//
// exports.helloWorld = functions.https.onRequest((request, response) => {
//  response.send("Hello from Firebase!");
// });
