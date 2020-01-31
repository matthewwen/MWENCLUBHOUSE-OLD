var header = undefined;
var body   = undefined;

var section_open = 0;
var section_url  = ['/www/html/about.html', '/www/html/work.html', '/www/html/school.html', '/www/html/project.html', '/www/html/miscellaneous.html'];
var heading_id   = ['#aboutme', '#workexp', '#school', '#project', '#miscell'];
var section_html = [null, null, null, null, null];

function init(){
    set_content_size();
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
    $(heading_id[section_open]).removeClass('selected');
    $('#div-body').html(html);
    section_open = new_selected;
    $(heading_id[section_open]).removeClass('notselected');
    $(heading_id[section_open]).addClass('selected');
}

function load_section(new_selected) {
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

function set_content_size() {
    var window_height = $(window).height();
    var header_heigth = $('#header-main').height();
    var footer_height = $('#homepage-footer').height();
    var min_height    = window_height - header_heigth - footer_height - footer_height - 5;

    $('#content-main').css('padding-top', header_heigth + "px");
    $('#div-body').css('min-height', min_height + "px");
}

function resize() {
    set_content_size();
}

window.onload   = init;
window.onresize = resize;