const urlParams = new URLSearchParams(window.location.search);
const gridID   = urlParams.get('grid');
const pkgName  = urlParams.get("name");

var password = Cookies.get("mwen_password");

function getDate() {
	var London = new Date().toLocaleString("en-US", {timeZone: "Europe/London"});
	var now = new Date(London);
	var pad = function(num) {
		var norm = Math.abs(Math.floor(num));     
		return (norm < 10 ? '0' : '') + norm;
	};
	return pad(now.getMonth()+1) + 
		pad(now.getDate()) + 
		now.getFullYear() + 'T' + 
		pad(now.getHours()) + 
		pad(now.getMinutes()) + 
		pad(now.getSeconds());
}

var date = getDate();

console.log("start of load\n");
$.ajax({type: 'GET',
		headers: {
			'authorization': $.sha256(JSON.stringify({'time': date, 'password': password})),
			'expires': date,
		},
		url: '/mwenCreatePROJ?pkg-name='+ pkgName + '&grid-name=' + gridID,
		contentType: "plain/text",
		success: function(result) {
			var obj = JSON.parse(result);
			console.log(result);
			if (obj.canEdit) {
				window.location.href = '/pageview?name=' + obj.href;
			}
			else {
				window.location.href = "/www/html/404.html";
			}
		}
});
