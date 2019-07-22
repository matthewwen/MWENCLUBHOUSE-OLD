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

    animate_header();
}

function add_alert(view, msg) {
    view.addEventListener("click", function(evt){
        alert(msg);
    })
}

function animate_header() {
    header  = document.querySelector(".header"  );
    body    = document.querySelector(".body"    );

    window.addEventListener('scroll', function(evt) {
        var pos = this.document.documentElement.scrollTop;
        if (pos === 0) {
            this.console.log(pos);
            header.style.height = "100vh";
        }
        else {
            this.console.log("set pxl: " + pos);
            header.style.height = "48px";
        }

    });
}

$(function() {
    console.log("Hello JQuery");
});

window.onload = init;
var parent = document.getElementsByClassName("main");
var p = document.createElement("p");
var n = document.createTextNode("Website is a work in progress");
p.appendChild(n);
parent[0].appendChild(p);