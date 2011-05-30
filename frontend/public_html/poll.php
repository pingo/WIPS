<?php

require_once 'config.inc';
require_once 'util.inc';
require_once 'xmlrpc.inc';

spl_autoload_extensions('.class.inc');
spl_autoload_register();

$rpc = new XMLRPC($cfg_rpc);

$time = (int)$_GET['time'];
$ajax = isset($_GET['ajax']);
list($major, $minor) = explode('.', $_GET['mote'], 2);
# list($major, $minor) = explode('.', '72.0', 2);

header('Content-Type: text/html');
header('X-Time: ' . (int)gmdate('U'));

$wait = !$ajax || $rpc->waitForData($time);

if ($wait)
{
	$db = new PDO($cfg_database);

	$stmt = $db->prepare('SELECT * FROM `event` INNER JOIN `mote` USING (`major`, `minor`) WHERE `major` = ? AND `minor` = ? AND `time` > ?  ORDER BY `time` DESC');
	$stmt->execute(array($major, $minor, $time + 2));

	$rows = $stmt->fetchAll();
}
else
	$rows = array();

if (count($rows) > 0)
{
	foreach ($rows as $row)
	{
		$divs[] = array
			(
				'div',
				'class' => 'logEntry ' . ($row['value'] ? 'occupied' : 'unoccupied'),

				array('span', 'class' => 'time', date('Y-m-d H:i:s', $row['time'])),
				array('span', 'class' => 'rssi', $row['rssi']),
				array('span', 'class' => 'hops', $row['hops']),
				array('span', 'Room is ' . ($row['value'] ? 'occupied' : 'empty'))
			);

		if (!isset($status))
			$status = $row['value'];
	}

	header('X-Occupied: ' . ($status ? 'yes' : 'no'));
}

if (!$ajax)
{
	$dk = new DOMKitObject('details.xml');

	$dk->insert('form', array(array('input', 'type' => 'hidden', 'name' => 'mote', 'value' => $_GET['mote'])));
	$dk->insert('status', array(array('img', 'id' => 'bulb', 'src' => 'images/light' . ($status ? 'on' : 'off') . '.png')));

	if (count($divs) > 0)
		$dk->insert('events', $divs);

	echo $dk->saveXML();
}
else
{
	if (count($divs) > 0)
	{
		$dk = new DOMKitObject('log.xml');
		$dk->insert('log', $divs);

		echo $dk->saveXML();
	}
}

