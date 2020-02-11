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

$.ajax({type: 'GET',
		headers: {
			'authorization': $.sha256(JSON.stringify({'time': date, 'password': password})),
			'grid-name': gridID,
			'pkg-name': pkgName,
			'mwen-date': date,
		},
		url: '/mwenCreatePROJ',
		success: function(result) {console.log(result);}
});
