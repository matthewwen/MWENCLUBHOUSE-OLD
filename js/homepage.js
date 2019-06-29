function init(){
    var aboutme = document.querySelector("#aboutme");
    var workexp = document.querySelector("#workexp");
    var school  = document.querySelector("#school");
    var project = document.querySelector("#project");
    var miscell = document.querySelector("#miscell");

    add_alert(aboutme, "about me");
    add_alert(workexp, "work experience");
    add_alert(school, "school");
    add_alert(project, "project");
    add_alert(miscell, "miscellaneous");
}

function add_alert(view, msg) {
    view.addEventListener("click", function(evt){
        alert(msg);
    })
}

do {
    window.onload = init;

}while(false);