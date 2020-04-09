var content_list = null;
get_all_pageview(callback=function(result){
    content_list = null;
    console.log(result);
    var list = result.data;
    console.log(result.data.length);
    for (var i = 0; i < list.length; i++) {
        $("#pageview_table").append("<tr>\
        <td><a href='/pageview/" + list[i].ref + "'>" + list[i].ref + "</a></td>\
        <td class='table_option " + ((list[i].parent) == undefined ? "warning": "") + "'>" + list[i].parent + "</td>\
        <td class='" + (list[i].html == undefined ? "warning": "") + "'>" + list[i].html + "</td>\
        <td class='"+ (list[i].visible ? "true": "false") + "'>" + (list[i].visible ? "True": "False") + "</td>\
        </tr>");
    }
});