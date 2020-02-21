var ref = "main";
const cjson = Cookies.get();
const urlParams = new URLSearchParams(window.location.search);
var section_open = Number.isNaN(parseInt(cjson.section_open)) ? 0 : parseInt(cjson.section_open);
section_open = urlParams.get('section') == null ? section_open : urlParams.get('section') % 5;

Cookies.set("mwen_pkgName", "main");

// responses from server
var section_url = ['/www/html/home.html', '/www/html/work.html', '/www/html/school.html', '/www/html/project.html', '/www/html/about.html'];
var heading_id = ['#home-menu', '#workexp-menu', '#school-menu', '#project-menu', '#about-menu'];
var title_name = ['Home', 'Work', 'School', 'Projects', 'About']
var section_html = [null, null, null, null, null];

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
	add_create_page(password);
	resizeAllGridItems();
	add_content_gpage();
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

window.onload = init;

// all webpages should check if they have admin privalges
var MWENKEYSTART = 0;
$(document).keypress(function (event) {
	console.log('key press: ' + MWENKEYSTART + ", char code: " + event.charCode);
	var edit_keyword = [109, 119, 101, 110];
	if (event.charCode == edit_keyword[MWENKEYSTART]) {
		MWENKEYSTART++;
	}
	else {
		MWENKEYSTART = 0;
	}

	if (MWENKEYSTART == 4) {
		MWENKEYSTART = 0;
		var password = prompt("Hey Vikram. What's up", "password");
		Cookies.set("mwen_password", password);
		add_create_page(password);
	}
});
