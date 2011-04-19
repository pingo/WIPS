<?php

require_once 'util.inc';

spl_autoload_extensions('.class.inc');
spl_autoload_register();

$main = new DOMKitObject('details.xml');

$db = new PDO('sqlite:../../backend/backend.db');

list($major, $minor) = explode('.', $_GET['mote'], 2);

$stmt = $db->prepare('SELECT * FROM `event` INNER JOIN `mote` ON `major` = `mote_major` AND `minor` = `mote_minor` WHERE `mote_major` = ? AND `mote_minor` = ? ORDER BY `time` DESC');
$stmt->execute(array($major, $minor));

$top = $stmt->fetch();

if (empty($top['name']))
	$top['name'] = 'Unnamed';

if ($top['value'])
	$image = array('img', 'id' => 'status', 'src' => '/images/lighton.png');
else
	$image = array('img', 'id' => 'status', 'src' => '/images/lightoff.png');

$main->insert('status', array($image));
$main->insertText('name',     $top['name']);
$main->insertText('address',  'Mote ' . $top['mote_major'] . '.' . $top['mote_minor']);
$main->insertText('lastseen', date('Y-m-d H:m:s', $top['time']) . ' (' . time_ago($top['time']) . ')');
$main->insertText('hopsavg',  '?');

$main->insert('form', array
	(
		array('input', 'type' => 'hidden', 'name' => 'mote', 'value' => $_GET['mote']),
		array('input', 'type' => 'text', 'name' => 'name', 'value' => !empty($top['name']) ? $top['name'] : 'Room ' . $_GET['mote']),
	));

foreach ($stmt as $row)
{
	$html_rows[] =
		array
		(
			'tr',
			array('td', 'class' => 'time', date('Y-m-d H:m:s', $row['time'])),
			array('td', 'Became ' . ($row['value'] ? 'occupied' : 'unoccupied')),
		);

	$tot_events++;
	$tot_hops += $row['hops'];
}

$main->insertText('noevents', $tot_events);

if ($html_rows)
	$main->insert('events', $html_rows);

echo $main->saveXML();

