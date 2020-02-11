const cjson = Cookies.get();
const urlParamsLoading  = new URLSearchParams(window.location.search);
var section_open        = urlParamsLoading.get('name');
var password            = Cookies.get('mwen_password');

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

console.log(section_open);

$.ajax({type: 'GET',
	headers: {
		'authorization': $.sha256(JSON.stringify({'time': getDate(), 'password': password})),
		'mwen-date': getDate()
	},
	url: '/pageview?name=' + section_open,
	contentType: "application/json",
	success: function (result) {
		var doc = (new DOMParser()).parseFromString(result, "text/html");
		$('head').html(doc.querySelector('head').innerHTML);
		$('body').html(doc.querySelector('body').innerHTML);
	},
});