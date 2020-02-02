var section_open = 0;
var section_url  = ['/www/html/home.html', '/www/html/work.html', '/www/html/school.html', '/www/html/project.html', '/www/html/about.html'];
var heading_id   = ['#home-menu', '#workexp-menu', '#school-menu', '#project-menu', '#about-menu'];
var title_name   = ['Home', 'Work', 'School', 'Projects', 'About']
var section_html = [null, null, null, null, null];

function init(){
	load_section(0);

	$(heading_id[0]).click(function() {
		load_section(0);
	});

	$(heading_id[1]).click(function() {
		load_section(1);
	});

	$(heading_id[2]).click(function() {
		load_section(2);
	});

	$(heading_id[3]).click(function() {
		load_section(3);
	});

	$(heading_id[4]).click(function() {
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
		$.ajax({type: 'GET',
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

window.onload   = init;
