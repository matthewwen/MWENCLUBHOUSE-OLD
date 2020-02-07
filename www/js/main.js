var section_open = 0;
var section_url  = ['/www/html/home.html', '/www/html/work.html', '/www/html/school.html', '/www/html/project.html', '/www/html/about.html'];
var heading_id   = ['#home-menu', '#workexp-menu', '#school-menu', '#project-menu', '#about-menu'];
var title_name   = ['Home', 'Work', 'School', 'Projects', 'About']
var section_html = [null, null, null, null, null];
var edit_keyword = [109, 119, 101, 110];
var key_start    = 0;
var password     = null;
var create_link  = null;
var can_edit     = false;

function resizeGridItem(item){
	var grid = document.getElementsByClassName("mwen-grid")[0];
	var rowHeight = parseInt(window.getComputedStyle(grid).getPropertyValue('grid-auto-rows'));
	var rowGap = parseInt(window.getComputedStyle(grid).getPropertyValue('grid-row-gap'));
	var rowSpan = Math.ceil((item.querySelector('.content').getBoundingClientRect().height+rowGap)/(rowHeight+rowGap) + 1);
	item.style.gridRowEnd = "span "+rowSpan;
}

function resizeAllGridItems(){
	var allItems = document.getElementsByClassName("item");
	for(var x=0;x<allItems.length;x++){
		resizeGridItem(allItems[x]);
	}
}

function add_create_link(password) {
	if (can_edit == false) {
		$.ajax({type: 'GET',
			headers: {
				'authorization': $.sha256(JSON.stringify({'time': getDate(), 'password': password})),
				'mwen-date': getDate()
			},
			url: '/webedit',
			contentType: "application/json",
			success: function (result) {
				console.log("result: " + result);
				var j = JSON.parse(result);
				can_edit = j.canEdit;
				if (can_edit) {
					add_create_link(password);
				}
			},
		});
	}
	else {
		if (create_link == null) {
			$.ajax({
				type: 'GET',
				url: 'www/html/template/add.html',
				contentType: "text/html",
				success: function (result) {
					create_link = result;
					if (create_link != null) {
						add_create_link("password");
					}
					else {
						console.log("Issue Communicating with the server!");
					}
				}
			});
		}
		else {
			var allItems = document.getElementsByClassName("mwen-grid");
			console.log("length: " + allItems.length);
			for (var x=0; x < allItems.length; x++) {
				var temp = document.createElement('div');
				temp.innerHTML = create_link;
				console.log("adding create element: " + x);
				allItems[allItems.length - 1 - x].prepend(temp.firstChild);
				resizeAllGridItems();
			}
		}
	}
}


function init() {
	load_section(section_open);

	$(heading_id[0]).click(function () {
		$('#div-body').html("");
		load_section(0);
	});

	$(heading_id[1]).click(function () {
		$('#div-body').html("");
		load_section(1);
	});

	$(heading_id[2]).click(function () {
		$('#div-body').html("");
		load_section(2);
	});

	$(heading_id[3]).click(function () {
		$('#div-body').html("");
		load_section(3);
	});

	$(heading_id[4]).click(function () {
		$('#div-body').html("");
		load_section(4);
	});
}

function load_body(html, new_selected) {
	$('#div-body').html(html);
	section_open = new_selected;	
	add_create_link("");
	resizeAllGridItems();
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

$(document).keypress(function (event) {
	if (event.charCode == edit_keyword[key_start]) {
		key_start++;
	}
	else {
		key_start = 0;
	}

	if (key_start == 4) {
		key_start = 0;
		password = prompt("Hey Vikram. What's up", "password");
		add_create_link(password);
	}
});

window.onload = init;
window.onresize = function () {
	resizeAllGridItems();
};
