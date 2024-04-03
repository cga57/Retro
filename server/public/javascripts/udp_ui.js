"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {

	window.setInterval(function(){
		sendCommandViaUDP("update");
	  }, 500);

	//   window.setInterval(function(){
	// 	sendCommandViaUDP("proc");
	//   }, 1000);

	$('#volumeDown').click(function(){
		sendCommandViaUDP("-volume");
	});
	$('#volumeUp').click(function(){
		sendCommandViaUDP("+volume");
	});
	$('#stop').click(function(){
		sendCommandViaUDP("stop");
	});
	$('#PausePlay').click(function(){
		sendCommandViaUDP("pause/play");
	});
	$('#Next_Song').click(function(){
		sendCommandViaUDP("next");
	});
	
	socket.on('commandReply', function(result) {
		var splitArray = result.split("-")
		if(splitArray[1] == "v"){
			document.getElementById("volumeid").value = splitArray[0];
		} if (splitArray[1] == "t"){
			document.getElementById("tempoid").value = splitArray[0];
		} if (splitArray[1] == "change"){
			if (splitArray[0] == "0"){
				document.getElementById("modeid").textContent = "Rock #1";
			}
			else if (splitArray[0] == "1"){
				document.getElementById("modeid").textContent = "Rock #2";
			}
			else{
				document.getElementById("modeid").textContent = "None"
			}
			
		}
		$('#error-box').hide();

	});

	socket.on('commandUpdate', function(result) {
		var splitArray = result.split(",")
		document.getElementById("volumeid").value = splitArray[1];
		document.getElementById("latId").innerHTML = splitArray[2];
		document.getElementById("longId").innerHTML = splitArray[3];
		console.log(splitArray[4]);
		switch(splitArray[4]){
			case '0':
				document.getElementById("carModeId").innerHTML = "SPORTS";
				break;
			case '1':
				document.getElementById("carModeId").innerHTML = "ECO";
				break;
			case '2':
				document.getElementById("carModeId").innerHTML = "CHANING";
				break;
			case '3':
				document.getElementById("carModeId").innerHTML = "REVERSED";
				break;
			case '4':
				document.getElementById("carModeId").innerHTML = "TOO_CLOSE_REVERSED";
				break;
			default:
				break;
		}
		
		
		console.log(splitArray[2]);
		console.log(splitArray[3]);

		
			
			$('#error-box').hide();

	});

	socket.on('daError', function(result) {
		document.getElementById("error-box").style.display= 'block';
	});
	
});

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);
};