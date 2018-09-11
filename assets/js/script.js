var count = 0;
var ledStatus = 0;

var databaseRef = firebase.database().ref();

document.getElementById("toggleLightButton").addEventListener("click", function(){
  if(count % 2 == 0){
    ledStatus = 1;
  }else ledStatus = 0;
  count++;
  databaseRef.child("status").set(ledStatus);
  console.log(ledStatus);
});
