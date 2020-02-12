var ref = "main";
const cjson = Cookies.get();
const urlParams = new URLSearchParams(window.location.search);
var section_open = Number.isNaN(parseInt(cjson.section_open)) ? 0 : parseInt(cjson.section_open);
section_open = urlParams.get('section') == null ? section_open : urlParams.get('section') % 5;
var password = Cookies.get('mwen_password');

Cookies.set("mwen_pkgName", "main");

// responses from server
var section_url = ['/www/html/home.html', '/www/html/work.html', '/www/html/school.html', '/www/html/project.html', '/www/html/about.html'];
var heading_id = ['#home-menu', '#workexp-menu', '#school-menu', '#project-menu', '#about-menu'];
var title_name = ['Home', 'Work', 'School', 'Projects', 'About']
var section_html = [null, null, null, null, null];
var edit_keyword = [109, 119, 101, 110];
var key_start = 0;
var create_link = null;
var can_edit = false;

function resizeGridItem(item) {
	var grid = document.getElementsByClassName("mwen-grid")[0];
	var rowHeight = parseInt(window.getComputedStyle(grid).getPropertyValue('grid-auto-rows'));
	var rowGap = parseInt(window.getComputedStyle(grid).getPropertyValue('grid-row-gap'));
	var rowSpan = Math.ceil((item.querySelector('.content').getBoundingClientRect().height + rowGap) / (rowHeight + rowGap) + 1);
	item.style.gridRowEnd = "span " + rowSpan;
}

function resizeAllGridItems() {
	var allItems = document.getElementsByClassName("item");
	for (var x = 0; x < allItems.length; x++) {
		resizeGridItem(allItems[x]);
	}
}

function add_create_link(password) {
	if (password == null) {
		return;
	}
	if (can_edit == false) {
		$('.addItem').remove();
		$.ajax({
			type: 'GET',
			headers: {
				'authorization': $.sha256(JSON.stringify({ 'time': getDate(), 'password': password })),
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
					can_edit = false;
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
						add_create_link(password);
					}
				}
			});
		}
		else {
			var allItems = document.getElementsByClassName("mwen-grid");
			for (var x = 0; x < allItems.length; x++) {
				// create div element
				var temp = document.createElement('div');
				temp.innerHTML = create_link;
				temp = temp.firstChild;

				// put element inside grid
				allItems[x].prepend(temp);
				resizeAllGridItems();

				// activate button
				temp = temp.getElementsByClassName("mdc-fab")[0];
				mdc.ripple.MDCRipple.attachTo(temp);
				temp.href = "createpkg?grid=" + allItems[x].getAttribute('id') + '&name=main';
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
	add_create_link(password);
	resizeAllGridItems();

	function add_content_grid() {
		$('.dynamic').remove();
		console.log("main\n");
		var allItems = document.getElementsByClassName("mwen-grid");
		for (var x = 0; x < allItems.length; x++) {
			var gridName = allItems[x].getAttribute('id');
			$.ajax({type: 'GET',
					url: '/mwenGetPROJ?pkg-name='+ref+'&grid-name='+gridName,
		            success: function(result){
						var json = JSON.parse(result);
						if (json == null || json.data == null)  {
							return;
						}
						if (json.gridName == null) {
							return;
						}
						var gridName = json.gridName;
						for (var i = 0; i < json.data.length; i++) {

							// create div element
							console.log(json.data[i].body);
							var content = document.createElement('div');
							content.setAttribute('class', 'content');
							content.innerHTML = json.data[i].body;

							var item = document.createElement('a');
							item.setAttribute('class', 'item dynamic');
							item.setAttribute('href', 'pageview?name=' + json.data[i].ref)
							item.appendChild(content);

							// put element inside grid
							document.getElementById(gridName).append(item);
							resizeAllGridItems();
						}
					}});
		}
	}

	add_content_grid();
}

function load_section(new_selected) {
	Cookies.set('section_open', new_selected);
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
	var London = new Date().toLocaleString("en-US", { timeZone: "Europe/London" });
	var now = new Date(London);
	var tzo = (now.getTimezoneOffset());
	var pad = function (num) {
		var norm = Math.abs(Math.floor(num));
		return (norm < 10 ? '0' : '') + norm;
	};
	return pad(now.getMonth() + 1) +
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
		Cookies.set("mwen_password", password);
		add_create_link(password);
	}
});

window.onload = init;
window.onresize = function () {
	resizeAllGridItems();
};