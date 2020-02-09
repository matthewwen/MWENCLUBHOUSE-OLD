const urlParams = new URLSearchParams(window.location.search);
const myParam   = urlParams.get('grid');
console.log("my param: " + myParam);

var dobj = {'gridName': myParam};
var jobj = {'data': JSON.stringify(dobj)};
var password = Cookies.get("mwen_password");

function getDate() {
	var London = new Date().toLocaleString("en-US", {timeZone: "Europe/London"});
	var now = new Date(London);
	var tzo = (now.getTimezoneOffset());
	var gmt = now.getTime();
	var dif = tzo >= 0 ? '+' : '-';
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
$.ajax({type: 'PUT',
        headers: {
            'authorization': $.sha256(JSON.stringify({'time': date, 'password': password})),
            'mwen-date': date 
        },
        url: '/create',
        data: jobj,
        success: function(result) {
            console.log(result);
        }
});
