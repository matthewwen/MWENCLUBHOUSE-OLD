var section_open = 4;
var section_url = ['/www/html/home.html', '/www/html/work.html', '/www/html/school.html', '/www/html/project.html', '/www/html/about.html'];
var heading_id = ['#home-menu', '#workexp-menu', '#school-menu', '#project-menu', '#about-menu'];
var title_name = ['Home', 'Work', 'School', 'Projects', 'About']
var section_html = [null, null, null, null, null];
var edit_keyword = [109, 119, 101, 110];
var key_start = 0;
var password = null;

function init() {
	load_section(section_open);

	$(heading_id[0]).click(function () {
		load_section(0);
	});

	$(heading_id[1]).click(function () {
		load_section(1);
	});

	$(heading_id[2]).click(function () {
		load_section(2);
	});

	$(heading_id[3]).click(function () {
		load_section(3);
	});

	$(heading_id[4]).click(function () {
		load_section(4);
	});
}

function load_body(html, new_selected) {
	$('#div-body').html(html);
	section_open = new_selected;
}

function load_section(new_selected) {
	$('#heading-text').html(title_name[new_selected]);
	if (section_html[new_selected] == null) {
		$.ajax({
			type: 'GET',
			url: section_url[new_selected],
			success: function (result) {
				load_body(result, new_selected);
				section_html[new_selected] = result;
			}
		});
	}
	else {
		load_body(section_html[new_selected], new_selected);
	}
}

function getDate() {
	var London = new Date().toLocaleString("en-US", {timeZone: "Europe/London"});
	var now = new Date(London);
	var tzo = (now.getTimezoneOffset());
	console.log(tzo);
	var gmt = now.getTime();
	console.log(new Date(gmt - tzo).toString());
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

$(document).keypress(function (event) {
	if (event.charCode == edit_keyword[key_start]) {
		key_start++;
	}
	else {
		key_start = 0;
	}

	if (key_start == 4) {
		password = prompt("Hey Vikram. What's up", "password");
		var str = JSON.stringify({'time': getDate(), 'password': password});
		$.ajax({
			type: 'GET',
			headers: {
				'authorization': $.sha256(str),
				'mwen-date': getDate()
			},
			url: '/webedit',
			success: function (result) {
				var j = JSON.parse(result);
				if (j.canEdit) {
					console.log("can edit");
				}
			},
		});
		key_start = 0;
	}
});

window.onload = init;