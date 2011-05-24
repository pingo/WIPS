
var time;

function setLight(lit)
{
	$('#bulb').attr('src', 'images/light' + (lit ? 'on' : 'off') + '.png');
}

function updateValue()
{
	var form = this;

	$(form).find('.loading').each(function() { $(this).show(); });

	$.ajax({
		type     : 'POST',
		url      : $(this).attr('action'),
		dataType : 'text',
		error    :
			function(jqXHR, textStatus, errorThrown) {
				$(form).find('.loading').hide();
				$(form).find('.error').text('Request failed').show().delay(3000).fadeOut();
			},
		success  : 
			function(data, textStatus, jqXHR) {
				$(form).find('.loading').hide();
				$(form).find('.success').text('Value updated').show().delay(3000).fadeOut();
			},
		timeout  : 20000,
		data     : {
			ajax   : 1,
			mote   : moteAddress,
			action : $(this).find('input[name="action"]').val(),
			value  : $(this).find('input[name="value"]').val()
		}
	});

	return false;
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
	$('.ajaxForm').live('submit', updateValue);
});

