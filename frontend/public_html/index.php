<?php

require_once 'util.inc';
require_once 'config.inc';

spl_autoload_extensions('.class.inc');
spl_autoload_register();

$main = new DOMKitObject('main.xml');

$db = new PDO($cfg_database);

$result = $db->query('SELECT DISTINCT `name`, `major` || \'.\' || `minor` AS `address` FROM `mote` ORDER BY `name`');

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

