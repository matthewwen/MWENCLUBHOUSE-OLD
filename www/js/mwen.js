// for any grid on the page
function resizeAllGridItems() {
	var allItems = document.getElementsByClassName("item");
	for (var x = 0; x < allItems.length; x++) {
		var item = allItems[x];
		var grid = document.getElementsByClassName("mwen-grid")[0];
		var rowHeight = parseInt(window.getComputedStyle(grid).getPropertyValue('grid-auto-rows'));
		var rowGap = parseInt(window.getComputedStyle(grid).getPropertyValue('grid-row-gap'));
		var rowSpan = Math.ceil((item.querySelector('.content').getBoundingClientRect().height + rowGap) / (rowHeight + rowGap) + 1);
		item.style.gridRowEnd = "span " + rowSpan;
	}
}
window.addEventListener('resize', resizeAllGridItems);

// add grid to reference to different web pages
function add_content_gpage() {
	if (!window.hasOwnProperty("ref")) {
		return;
	}
	$('.dynamic').remove();
	var allItems = document.getElementsByClassName("mwen-grid");
	for (var x = 0; x < allItems.length; x++) {
		var gridName = allItems[x].getAttribute('id');
		$.ajax({
			type: 'GET',
			url: '/mwenGetPROJ?pkg-name=' + ref + '&grid-name=' + gridName,
			contentType: 'application/json',
			headers: {"Accept": "application/json; odata=verbose"},
			credentials: 'include',
			dataType: "json",
			success: function (json) {
				if (json == null || json.data == null) {
					return;
				}
				if (json.gridName == null) {
					return;
				}
				var gridName = json.gridName;
				for (var i = 0; i < json.data.length; i++) {

					// create div element
					var content = document.createElement('div');
					content.setAttribute('class', 'content');
					content.innerHTML = json.data[i].body;

					var item = document.createElement('a');
					item.setAttribute('class', 'item dynamic item-hover');
					item.setAttribute('href', 'pageview/' + json.data[i].ref);
					item.appendChild(content);

					// put element inside grid
					document.getElementById(gridName).append(item);
					resizeAllGridItems();
				}
			},
			failure: function () {
				console.log("failed");
			}
		});
	}
}

function get_all_pageview(callback=function(result){console.log(result)}) {
	$.ajax({type: 'GET',
			url: '/apipageview/all',
			contentType: 'application/json',
			headers: {"Accept": "application/json; odata=verbose"},
			credentials: 'include',
			dataType: "json",
			success: callback
	});
}

