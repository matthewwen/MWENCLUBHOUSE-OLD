var aboutme = undefined;
var workexp = undefined; 
var school  = undefined;
var project = undefined;
var miscell = undefined;

var header = undefined;
var body   = undefined;

function init(){
    aboutme = document.querySelector("#aboutme");
    workexp = document.querySelector("#workexp");
    school  = document.querySelector("#school");
    project = document.querySelector("#project");
    miscell = document.querySelector("#miscell");

    add_alert(aboutme, "about me");
    add_alert(workexp, "work experience");
    add_alert(school, "school");
    add_alert(project, "project");
    add_alert(miscell, "miscellaneous");

    set_content_size();
}

function add_alert(view, msg) {
    view.addEventListener("click", function(evt){
        alert(msg);
    })
}

function set_content_size() {
    var window_height = $(window).height();
    var header_heigth = $('#header-main').height();
    var footer_height = $('#homepage-footer').height();
    var min_height    = window_height - header_heigth - footer_height - footer_height - 10;

    $('#content-main').css('padding-top', header_heigth + "px");
    $('#div-body').css('min-height', min_height + "px");
}

function resize() {
    set_content_size();
}

window.onload   = init;
window.onresize = resize;