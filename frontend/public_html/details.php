<?php

require_once 'util.inc';

spl_autoload_extensions('.class.inc');
spl_autoload_register();

$main = new DOMKitObject('details.xml');

$db = new PDO('sqlite:../../backend/backend.db');

list($major, $minor) = explode('.', $_GET['mote'], 2);

$stmt = $db->prepare('SELECT * FROM `event` INNER JOIN `mote` USING (`major`, `minor`) WHERE `major` = ? AND `minor` = ? ORDER BY `time` DESC');
$stmt->execute(array($major, $minor));

$top = $stmt->fetch();

if ($top['value'])
	$image = array('img', 'id' => 'status', 'src' => '/images/lighton.png');
else
	$image = array('img', 'id' => 'status', 'src' => '/images/lightoff.png');

$main->insert('status', array($image));
$main->insertText('name', !empty($top['name']) ? $top['name'] : 'Unnamed room');
$main->insertText('address',  'Mote ' . $_GET['mote']);

if ($top['time'])
	$main->insertText('lastseen', date('Y-m-d H:i:s', $top['time']) . ' (' . time_ago($top['time']) . ')');
else
	$main->insertText('lastseen', 'Never');

$main->insert('form', array
	(
		array('input', 'type' => 'hidden', 'name' => 'mote', 'value' => $_GET['mote']),
		array('input', 'type' => 'text', 'name' => 'name', 'value' => !empty($top['name']) ? $top['name'] : 'Room ' . $_GET['mote']),
	));

$tot_events = 0;
$tot_hops = 0;

if (!empty($top))
{
	$arr  = array($top);
	$arr += $stmt->fetchAll();

	foreach ($arr as $row)
	{
		$html_rows[] =
			array
			(
				'tr',
				array('td', 'class' => 'time', date('Y-m-d H:i:s', $row['time'])),
				array('td', 'Became ' . ($row['value'] ? 'occupied' : 'unoccupied')),
			);

		$tot_events++;
		$tot_hops += $row['hops'];
	}
}

$main->insertText('noevents', (int)$tot_events);
$main->insertText('hopsavg', round((double)$tot_hops / $tot_events, 2));

if ($html_rows)
	$main->insert('events', $html_rows);

echo $main->saveXML();

