const MWENHEADER = document.querySelector('.main-header');

// for the header
window.addEventListener('scroll', () => {
	const scrollPos = window.scrollY;
	if (MWENHEADER != null) {
		if (scrollPos > 10) {
			MWENHEADER.classList.add('scrolled');
		}
		else {
			MWENHEADER.classList.remove('scrolled');
		}
	}
});

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

// get date for tokenization
function getDate() {
	var London = new Date().toLocaleString("en-US", { timeZone: "Europe/London" });
	var now = new Date(London);
	var pad = function (num) {
		var norm = Math.abs(Math.floor(num));
		return (norm < 10 ? '0' : '') + norm;
	};
	return pad(now.getMonth() + 1) + pad(now.getDate()) + now.getFullYear() + 'T' + pad(now.getHours()) + pad(now.getMinutes()) + pad(now.getSeconds());
}

// on the grids, add the create link page
var MWENTEMPLATECREATEPAGE = null;
function add_create_page(userPassword) {
	console.log("Hello There");
	add_create_page_helper(userPassword, false);
	function add_create_page_helper(userPasswordHelper, userCanEditHelper) {
		if (userPasswordHelper == null) {
			return;
		}
		if (userCanEditHelper == false) {
			var date = getDate();
			var auth = $.sha256(JSON.stringify({ 'time': date, 'password': userPassword }));
			console.log(date);
			console.log(auth);
			$('.addItem').remove();
			$.ajax({
				type: 'GET',
				url: '/webedit',
				headers: {
					"expires": date,
					"authorization":auth
				},
				success: function (result) {
					console.log("result: " + result);
					var j = JSON.parse(result);
					if (j.canEdit) {
						add_create_page_helper(userPasswordHelper, true);
					}
				},
			});
		}
		else {
			if (MWENTEMPLATECREATEPAGE == null) {
				$.ajax({
					type: 'GET',
					url: 'www/html/template/add.html',
					contentType: "text/html",
					success: function (result) {
						MWENTEMPLATECREATEPAGE = result;
						if (MWENTEMPLATECREATEPAGE != null) {
							add_create_page_helper(userPasswordHelper, true);
						}
					}
				});
			}
			else {
				var allItems = document.getElementsByClassName("mwen-grid");
				for (var x = 0; x < allItems.length; x++) {
					// create div element
					var temp = document.createElement('div');
					temp.innerHTML = MWENTEMPLATECREATEPAGE;
					temp = temp.firstChild;

					// put element inside grid
					allItems[x].prepend(temp);
					resizeAllGridItems();

					// activate button
					temp = temp.getElementsByClassName("mdc-fab")[0];
					mdc.ripple.MDCRipple.attachTo(temp);
					temp.href = "createpkg?grid=" + allItems[x].getAttribute('id') + '&name=main';
				}
			}
		}
	}
}

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
			contentType: 'plain/text',
			success: function (result) {
				var json = JSON.parse(result);
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
					item.setAttribute('class', 'item dynamic');
					item.setAttribute('href', 'pageview?name=' + json.data[i].ref);
					item.appendChild(content);

					// put element inside grid
					document.getElementById(gridName).append(item);
					resizeAllGridItems();
				}
			}
		});
	}
}

