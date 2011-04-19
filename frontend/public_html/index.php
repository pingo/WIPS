<?php

require_once 'util.inc';

spl_autoload_extensions('.class.inc');
spl_autoload_register();

$main = new DOMKitObject('main.xml');

$db = new PDO('sqlite:../../backend/backend.db');

$result = $db->query('SELECT DISTINCT * FROM (SELECT `mote_major`, `mote_minor`, `mote_major` || \'.\' || `mote_minor` AS `address`, `value` FROM `event` ORDER BY `time`) INNER JOIN `mote` ON `mote_major` = `major` AND `mote_minor` = `minor` GROUP BY `address`');

$main->insertText('js', 'var moteAddress = "' . $_GET['mote'] . '";');

if ($result === FALSE)
	die(var_export($db->errorInfo(), TRUE));

foreach ($result as $row)
{
	$named = !empty($row['name']);
	$name  = $named ? $row['name'] : 'Unnamed room';

	$html_rooms[] =
		array
		(
			'div',
			'class' => 'room' .
				($row['value'] ? ' occupied' : ' unoccupied') .
				($row['address'] == $_GET['mote'] ? ' current' : ''),

			array('a',
				'href' => '/?' . http_build_query(array('mote' => $row['address'])),
				array('div', 'class' => 'name' . (!$named ? ' unnamed' : ''), $name),
			),
			array('div', 'class' => 'address', 'Mote ' . $row['address']),
		);
}

$result->closeCursor();

if (isset($html_rooms))
	$main->insert('side', $html_rooms);

header('Content-Type: text/html; charset=UTF-8');
header('Content-Language: en');

echo $main->saveXML();

