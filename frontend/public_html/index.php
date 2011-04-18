<?php

require_once 'util.inc';

spl_autoload_extensions('.class.inc');
spl_autoload_register();

$main = new DOMKitObject('main.xml');

$main->insertText('title', 'Room status');

$db = new PDO('sqlite:../../backend/backend.db');

$result = $db->query('SELECT * FROM `event` ORDER BY `time` DESC');

$last_value = 0;

foreach ($result as $row)
{
	$html_rows[] =
		array
		(
			'tr',
			'class' => $row['value'] == '1' ? 'occupied' : 'unoccupied',

			array('td', time_ago($row['time']), 'title' => date('Y-m-d H:m:s', $row['time'])),
			array('td', 'Rum ' . $row['mote_major'] .  '.' . $row['mote_minor'] . ' blev ' . ($row['value'] == '1' ? 'upptaget' : 'ledigt')),
		);

	if (isset($last))
	{
		if ($last_value == '1')
			$total_occupied += ((int)$row['time'] - $last);

		$total_time += ((int)$row['time'] - $last);
	}

	$last_value = $row['value'];
	$last = $row['time'];
}

if ($last_value == '1')
{
	$total_occupied += (time() - $last);
	$free = TRUE;
}

$val = (double)$total_occupied / (double)$total_time;
$uri = 'http://chart.apis.google.com/chart?' .
	http_build_query(array
		(
			'chs'  => '300x225',
			'cht'  => 'p3',
			'chd'  => 's:FU',
			'chdl' => 'Ledigt|Upptaget',
			'chp'  => $val,
			'chtt' => 'Rum ' . $row['mote_major'] . '.' . $row['mote_minor'],
		));

$main->insert('chart', array('img', 'src' => $uri));
$main->insert('body', array('table', 'class' => 'log') + $html_rows);

header('Content-Type: text/html; charset=UTF-8');
header('Content-Language: en');

echo $main->saveXML();

