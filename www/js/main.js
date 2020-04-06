console.log("Hello There");

var ref = "main";
const cjson = Cookies.get();
const urlParams = new URLSearchParams(window.location.search);
var section_open = Number.isNaN(parseInt(cjson.section_open)) ? 0 : parseInt(cjson.section_open);
section_open = urlParams.get('section') == null ? section_open : urlParams.get('section') % 5;

Cookies.set("mwen_pkgName", "main");

var enterLogin = false;

// responses from server
var section_url = ['/www/html/home.html', '/www/html/work.html', '/www/html/school.html', '/www/html/project.html', '/www/html/about.html'];
var heading_id = ['.home-menu', '.workexp-menu', '.school-menu', '.project-menu', '.about-menu'];
var title_name = ['Home', 'Work', 'School', 'Projects', 'About']
var section_html = [null, null, null, null, null];

function switch_x() {
	var b1 = document.getElementById("bar1");
	var b2 = document.getElementById("bar2");
	var b3 = document.getElementById("bar3");
	if (b1.className === "bar1") {
		b1.className = "";
		b2.className = "";
		b3.className = "";
	}
	else {
		b1.className = "bar1";
		b2.className = "bar2";
		b3.className = "bar3";
	}
}

var menu_clicked = false;
function open_menu() {
	switch_x();
	var menu = document.getElementById("menu-option");
	menu_clicked = !menu_clicked;
	if (menu_clicked) {
		menu.style = "display: block;";
		setTimeout(function () {
			menu.className = "option-links fade_out";
		}, 300);
	}
	else {
		menu.className = "option-links fade_in";
		setTimeout(function () {
			menu.style = "display: none;";
		}, 300);
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
	var password = Cookies.get('mwen_password');
	resizeAllGridItems();
	add_content_gpage();
}

function load_section(new_selected) {
	// remove all section with the 
	var navList = document.getElementsByClassName("active-nav-link");
	for (var i = 0; i < navList.length; i++) {
		navList[i].classList.remove("active-nav-link");
	}
	$(heading_id[new_selected]).addClass("active-nav-link");

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

window.onload = init;

// all webpages should check if they have admin privalges
var MWENKEYSTART = 0;
$(document).keypress(function (event) {
	if (enterLogin && event.keyCode == 13) {
		// attempt login
		attempt_login();
	}
	else {
		var edit_keyword = [109, 119, 101, 110];
		if (event.charCode == edit_keyword[MWENKEYSTART]) {
			MWENKEYSTART++;
		}
		else {
			MWENKEYSTART = 0;
		}

		if (MWENKEYSTART == 4) {
			$('#popup-window').css("display", "block");
			enterLogin = true;
		}
	}
});

$('#popup-window').click(function(e) {
	if (e.target == this) {
		hide_login();
	}
});

$('#btn-cancel').click(hide_login);

function hide_login() {
	$('#popup-window').css("display", "none");
	enterLogin = false;
}

$('#btn-okay').click(attempt_login);

function attempt_login() {
	hide_login();
	var username = $('#input-username').val();
	var password = $('#input-password').val();

	// TODO should not set password as cookie
	// Cookies.set('mwen_password', password);
}

$(".option-link").click(open_menu);
$(window).resize(function() {
	if (menu_clicked) {
		open_menu();
	}
});

