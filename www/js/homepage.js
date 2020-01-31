var header = undefined;
var body   = undefined;

var section_open = 0;
var section_url  = ['/www/html/home.html', '/www/html/work.html', '/www/html/school.html', '/www/html/project.html', '/www/html/miscellaneous.html'];
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
    var window_width  = $(window).width();
    var body_width    = $('#div-body').width();
    var abme_width    = $('#about-me-side').width();
    var header_heigth = $('#header-main').height();
    var footer_height = $('#homepage-footer').height();
    var min_height    = window_height - header_heigth - footer_height - footer_height - 5;

    var margin_left = (window_width - body_width) / 2 - abme_width;

    if (margin_left > 0) {
        console.log("Hello There");
        $('#about-me-side').css("margin-left", margin_left + "px");
        $('#div-body').css('margin-left', 0 + 'px');
        $('#about-me-side').css('visibility', 'visible');
        $('#about-me-side').show();
    }
    else {
        console.log("Hello There hide");
        $('#div-body').css('margin-left', 'auto');
        $('#about-me-side').css('visibility', 'collapse');
        $('#about-me-side').hide();
    }

    $('#content-main').css('padding-top', header_heigth + "px");
    $('#div-body').css('min-height', min_height + "px");
    $('#-body').css('height', "auto");
}

function resize() {
    set_content_size();
}

window.onload   = init;
window.onresize = resize;
