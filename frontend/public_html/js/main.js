
function updateDetails(xhr, status)
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
			$('#main').empty().append($(xhr.responseText));

			
			break;

		default:
			break;
	}
}

function refresh()
{
	$.ajax({
		type     : 'GET',
		url      : '/details.php',
		complete : updateDetails,
		timeout  : 20000,
		data     : { mote : moteAddress }
	});

//	setTimeout(refresh, 5000);
}

$(document).ready(function()
{
	refresh();
});

