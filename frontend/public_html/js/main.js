
var time;

function setLight(lit)
{
	$('#bulb').attr('src', 'images/light' + (lit ? 'on' : 'off') + '.png');
}

function loadDetails(xhr, status)
{
	switch (status)
	{
		case 'timeout':
		case 'error':
		case 'abort':
		case 'parsererror':
			alert('Request failed: error parsing server response.');
			break;

		case 'success':
			time = xhr.getResponseHeader('X-Time');
			occupied = xhr.getResponseHeader('X-Occupied');

			if (occupied != null)
				setLight(occupied == 'yes');
			$('#main').empty().append($(xhr.responseText));
			refresh();
			break;

		default:
			break;
	}
}

function refreshDetails(xhr, status)
{
	switch (status)
	{
		case 'timeout':
		case 'error':
		case 'abort':
		case 'parsererror':
			alert('Request failed: error parsing server response.');
			break;

		case 'success':
			time = xhr.getResponseHeader('X-Time');
			occupied = xhr.getResponseHeader('X-Occupied');

			if (occupied != null)
				setLight(occupied == 'yes');

			$('#events').prepend($(xhr.responseText));
			refresh();
			break;

		default:
			break;
	}
}

function refresh()
{
	$.ajax({
		type     : 'GET',
		url      : '/poll.php',
		complete : refreshDetails,
		timeout  : 20000,
		data     : { mote : moteAddress, ajax : 1, time : time }
	});	
} 

function load()
{
	$.ajax({
		type     : 'GET',
		url      : '/poll.php',
		complete : loadDetails,
		timeout  : 20000,
		data     : { mote : moteAddress, time : time }
	});
}

$(document).ready(function()
{
	load();
});

