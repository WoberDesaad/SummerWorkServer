function startTime() {
	document.getElementById('Time').innerHTML = new Date()
	var t = setTimeout(startTime, 500);
}